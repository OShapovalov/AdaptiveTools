#pragma once

#include <vector>
#include <array>

class PTLInterval
{

public:
	PTLInterval(double iStart, double iEnd);

	PTLInterval();

	inline double GetStart() const {return _interval[0];};

	inline double GetEnd() const {return _interval[1];};

private:

	void Init(double iStart, double iEnd);

	std::array<double,2> _interval;
};