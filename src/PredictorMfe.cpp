
#include "PredictorMfe.h"

#include <iostream>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////

PredictorMfe::PredictorMfe( const InteractionEnergy & energy, OutputHandler & output )
	: Predictor(energy,output)
	, mfeInteractions()
	, reportedInteractions()
	, minStackingEnergy( energy.getBestE_interLoop() )
	, minInitEnergy( energy.getE_init() )
	, minDangleEnergy( energy.getBestE_dangling() )
	, minEndEnergy( energy.getBestE_end() )
{

}

////////////////////////////////////////////////////////////////////////////

PredictorMfe::~PredictorMfe()
{
}


////////////////////////////////////////////////////////////////////////////

void
PredictorMfe::
initOptima( const OutputConstraint & outConstraint )
{
	// resize to the given number of interactions if overlapping reports allowed
	mfeInteractions.resize(outConstraint.reportOverlap!=OutputConstraint::ReportOverlap::OVERLAP_BOTH ? 1 : outConstraint.reportMax
			, Interaction(energy.getAccessibility1().getSequence()
					,energy.getAccessibility2().getAccessibilityOrigin().getSequence())
			);

	// init all interactions to be filled
	for (InteractionList::iterator i = mfeInteractions.begin(); i!= mfeInteractions.end(); i++) {
		// initialize global E minimum : should be below 0.0
		i->energy = 0.0;
		// ensure it holds only the boundary
		if (i->basePairs.size()!=2) {
			i->basePairs.resize(2);
		}
		// reset boundary base pairs
		i->basePairs[0].first = RnaSequence::lastPos;
		i->basePairs[0].second = RnaSequence::lastPos;
		i->basePairs[1].first = RnaSequence::lastPos;
		i->basePairs[1].second = RnaSequence::lastPos;
	}

	// clear reported interaction ranges
	reportedInteractions.first.clear();
	reportedInteractions.second.clear();

}

////////////////////////////////////////////////////////////////////////////

void
PredictorMfe::
updateOptima( const size_t i1, const size_t j1
		, const size_t i2, const size_t j2
		, const E_type hybridE )
{
//	LOG(DEBUG) <<"energy( "<<i1<<"-"<<j1<<", "<<i2<<"-"<<j2<<" ) = "
//			<<hybridE;

	// check if nothing to be done
	if (mfeInteractions.size() == 0) {
		return;
	}

	// get final energy of current interaction
	E_type curE = energy.getE( i1,j1, i2,j2, hybridE );
//	LOG(DEBUG) <<"energy( "<<i1<<"-"<<j1<<", "<<i2<<"-"<<j2<<" ) = "
//			<<hybridE <<" : total = "<<curE;

	if (mfeInteractions.size() == 1) {
		if (curE < mfeInteractions.begin()->energy) {
	//		LOG(DEBUG) <<"PredictorMfe::updateOptima() : new mfe ( "
	//			<<i1<<"-"<<j1<<", "<<i2<<"-"<<j2<<" ) = " <<hybridE <<" : "<<curE;
			// store new global min
			mfeInteractions.begin()->energy = (curE);
			// store interaction boundaries
			// left
			mfeInteractions.begin()->basePairs[0] = energy.getBasePair(i1,i2);
			// right
			mfeInteractions.begin()->basePairs[1] = energy.getBasePair(j1,j2);
		}
	} else {

		// check if within range of already known suboptimals (< E(worst==last))
		if (curE < mfeInteractions.rbegin()->energy) {

			// identify sorted insertion position (iterator to position AFTER insertion)
			InteractionList::iterator toInsert = --(mfeInteractions.end());
			InteractionList::iterator insertSuccPos =
					std::upper_bound(mfeInteractions.begin(), toInsert
								, curE
								, Interaction::compareEnergy );

			// check if not already within list
			bool isToBeInserted = insertSuccPos == mfeInteractions.begin();
			if (!isToBeInserted) {
				// go to preceeding interaction to check for equivalence
				--insertSuccPos;
				// check if not equal
				isToBeInserted =
						// check energy
						! E_equal(curE,insertSuccPos->energy)
						// check boundaries
						&& insertSuccPos->basePairs.at(0) != energy.getBasePair(i1,i2)
						&& insertSuccPos->basePairs.at(1) != energy.getBasePair(j1,j2)
						;
				// restore insert position
				++insertSuccPos;
			}

			// insert current interaction into lowest energy interaction list
			if (isToBeInserted) {

				// overwrite last list element
				// set current energy
				toInsert->energy = (curE);
				// store interaction boundaries
				// left
				toInsert->basePairs[0] = energy.getBasePair(i1,i2);
				// right
				toInsert->basePairs[1] = energy.getBasePair(j1,j2);

				// relocate last element within list (no reallocation needed, just list-link-update)
				mfeInteractions.splice( insertSuccPos, mfeInteractions, toInsert );
			}

		}
	}
}

////////////////////////////////////////////////////////////////////////////

void
PredictorMfe::
reportOptima( const OutputConstraint & outConstraint )
{
	// number of reported interactions
	size_t reported = 0;

	// clear reported interaction ranges
	reportedInteractions.first.clear();
	reportedInteractions.second.clear();

	// check if non-overlapping output is wanted
	if (outConstraint.reportOverlap!=OutputConstraint::ReportOverlap::OVERLAP_BOTH) {
		// check if mfe is worth reporting
		Interaction curBest = *mfeInteractions.begin();
		while( curBest.energy < 0.0 && reported < outConstraint.reportMax ) {
			// report current best
			// fill interaction with according base pairs
			traceBack( curBest );
			// report mfe interaction
			output.add( curBest );
			// count
			reported++;

			// store ranges to ensure non-overlapping of next best solution
			switch( outConstraint.reportOverlap ) {
			case OutputConstraint::ReportOverlap::OVERLAP_BOTH :
				break;
			case OutputConstraint::ReportOverlap::OVERLAP_SEQ1 :
				reportedInteractions.second.insert( IndexRange(energy.getIndex2(*curBest.basePairs.begin()),energy.getIndex2(*curBest.basePairs.rbegin())) );
				break;
			case OutputConstraint::ReportOverlap::OVERLAP_SEQ2 :
				reportedInteractions.first.insert( IndexRange(energy.getIndex1(*curBest.basePairs.begin()),energy.getIndex1(*curBest.basePairs.rbegin())) );
				break;
			case OutputConstraint::ReportOverlap::OVERLAP_NONE :
				reportedInteractions.first.insert( IndexRange(energy.getIndex1(*curBest.basePairs.begin()),energy.getIndex1(*curBest.basePairs.rbegin())) );
				reportedInteractions.second.insert( IndexRange(energy.getIndex2(*curBest.basePairs.begin()),energy.getIndex2(*curBest.basePairs.rbegin())) );
				break;
			}

			// get next best
			getNextBest( curBest );
		}

	} // non-overlapping interactions

	else // overlapping interactions are allowed
	{
		// report all (possibly overlapping) interactions with energy below 0
		assert(outConstraint.reportMax <= mfeInteractions.size());
		for (InteractionList::iterator i = mfeInteractions.begin();
				reported < outConstraint.reportMax
				&& i!= mfeInteractions.end(); i++)
		{

			// check if interaction is better than no interaction (E==0)
			if (i->energy < 0.0) {
				// fill mfe interaction with according base pairs
				traceBack( *i );
				// report mfe interaction
				output.add( *i );
				// count
				reported++;
			}
		}
	} // overlapping interactions

	// check if nothing was worth reporting but report was expected
	if (reported == 0 && outConstraint.reportMax > 0 ) {
		// -> no preferable interactions found !!!
		// replace mfeInteraction with no interaction
		mfeInteractions.begin()->clear();
		mfeInteractions.begin()->energy = 0.0;
		// report mfe interaction
		output.add( *(mfeInteractions.begin()) );
	}

}

////////////////////////////////////////////////////////////////////////////

