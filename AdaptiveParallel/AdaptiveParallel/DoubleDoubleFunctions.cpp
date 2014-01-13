#include "DoubleDoubleFunctions.h"
#include <Windows.h>

double Tf2::Evaluate( const double& iParam ) const
{
	//Sleep(5);
	return iParam*iParam;
}

void Tf2::GetIntervals(std::vector<TplInterval>& oIntervals) const
{
	oIntervals.clear();
	oIntervals.push_back(TplInterval(0,1));
}


double Tfsin::Evaluate( const double& iParam ) const
{
	//Sleep(5); 
	return std::abs(sin(31*iParam));
}

void Tfsin::GetIntervals(std::vector<TplInterval>& oIntervals) const
{
	oIntervals.clear();
	oIntervals.push_back(TplInterval(0,1));
}
