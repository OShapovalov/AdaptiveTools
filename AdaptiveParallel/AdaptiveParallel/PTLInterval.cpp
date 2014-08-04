#include "PTLInterval.h"

PTLInterval::PTLInterval(double iStart, double iEnd)
{
	Init(iStart, iEnd);
}

PTLInterval::PTLInterval()
{
	Init(0.0,0.0);
};

void PTLInterval::Init(double iStart, double iEnd)
{
	//_interval.resize(2);
	_interval[0] = iStart;
	_interval[1] = iEnd;
}