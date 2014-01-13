#include "TplInterval.h"

TplInterval::TplInterval(double iStart, double iEnd)
{
	Init(iStart, iEnd);
}

TplInterval::TplInterval()
{
	Init(0.0,0.0);
};

void TplInterval::Init(double iStart, double iEnd)
{
	//_interval.resize(2);
	_interval[0] = iStart;
	_interval[1] = iEnd;
}