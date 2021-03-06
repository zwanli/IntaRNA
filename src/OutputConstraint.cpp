
#include "OutputConstraint.h"


/////////////////////////////////////////////////////////////////////////////

OutputConstraint::OutputConstraint(
		  const size_t reportMax
		, const ReportOverlap reportOverlap
		, const E_type maxE
		, const E_type deltaE )
 :
	  reportMax(reportMax)
	, reportOverlap(reportOverlap)
	, maxE(maxE)
	, deltaE(deltaE)
{
	if(deltaE < (E_type)0.0) throw std::runtime_error("OutputConstraint(deltaE="+toString(deltaE)+") not >= 0.0");
}

/////////////////////////////////////////////////////////////////////////////

OutputConstraint::~OutputConstraint()
{
}

/////////////////////////////////////////////////////////////////////////////

