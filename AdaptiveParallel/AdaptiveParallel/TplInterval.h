#pragma once

#include <vector>
#include <array>

class TplInterval
{

public:
	TplInterval(double iStart, double iEnd);

	TplInterval();

	inline double GetStart() const {return _interval[0];};

	inline double GetEnd() const {return _interval[1];};

private:

	void Init(double iStart, double iEnd);

	std::array<double,2> _interval;
};