#include "DoubleDoubleFunctions.h"
#include <Windows.h>

double Tf2::Evaluate( const double& iParam ) const
{
	//Sleep(5);
	return iParam*iParam;
}

void Tf2::GetIntervals(std::vector<Interval>& oIntervals) const
{
	oIntervals.clear();
	oIntervals.push_back(Interval(0,1));
}


double Tfsin::Evaluate( const double& iParam ) const
{
	//Sleep(5); 
	return std::abs(sin(31*iParam));
}

void Tfsin::GetIntervals(std::vector<Interval>& oIntervals) const
{
	oIntervals.clear();
	oIntervals.push_back(Interval(0,1));
}
