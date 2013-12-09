#pragma once

#include <vector>

class Interval
{

public:
	Interval(double iStart, double iEnd);

	Interval();

	inline double GetStart() const {return _interval[0];};

	inline double GetEnd() const {return _interval[1];};

private:

	void Init(double iStart, double iEnd);

	std::vector<double> _interval;
};