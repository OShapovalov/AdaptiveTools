#pragma once
#include <functional>

class AbstractParallel
{
public:

	static int GetNumberOMP(std::function<bool (void)> f);

	static double GetTimeForOmpOrSerial( std::function<bool (void)> f, int n, bool omp);

	static double GetTime();
};