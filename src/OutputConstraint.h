
#ifndef OUTPUTCONSTRAINT_H_
#define OUTPUTCONSTRAINT_H_

#include "general.h"

/**
 *
 * Data structure that contains all constraints to be applied to (suboptimal)
 * output generation.
 *
 * @author Martin Mann
 *
 */
class OutputConstraint
{

public:

	//! different possibilities to en-/disable overlapping of interaction sites
	//! if suboptimal solutions are enumerated
	enum ReportOverlap {
		OVERLAP_NONE = 0,
		OVERLAP_SEQ1 = 1,
		OVERLAP_SEQ2 = 2,
		OVERLAP_BOTH = 3
	};


public:

	//! the maximal number of (sub)optimal interactions to be reported to the output handler
	const size_t reportMax;

	//! defines whether and where overlapping interaction sites are allowed for reporting
	const ReportOverlap reportOverlap;

public:

	/**
	 * Construction of an output constraint
	 *
	 * @param reportMax the maximal number of (sub)optimal interactions to be
	 *            reported to the output handler
	 * @param reportOverlap defines whether and where overlapping interaction
	 *            sites are allowed for reporting
	 */
	OutputConstraint(	  const size_t reportMax = 1
						, const ReportOverlap reportOverlap = OVERLAP_BOTH );

	//! destruction
	virtual ~OutputConstraint();
};

#endif /* OUTPUTCONSTRAINT_H_ */
