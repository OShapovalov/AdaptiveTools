#include "Interval.h"

Interval::Interval(double iStart, double iEnd)
{
	Init(iStart, iEnd);
}

Interval::Interval()
{
	Init(0.0,0.0);
};

void Interval::Init(double iStart, double iEnd)
{
	//_interval.resize(2);
	_interval[0] = iStart;
	_interval[1] = iEnd;
}