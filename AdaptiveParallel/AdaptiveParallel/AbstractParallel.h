#pragma once
#include <functional>
#include <windows.h>

class AbstractParallel
{
public:

	static int GetNumberOMP(std::function<bool (void)> f);

	static double GetTimeForOmpOrSerial( std::function<bool (void)> f, int n, bool omp);

	static double GetTime();
};

template <class Function>
__int64 time_call(Function&& f)
{
    __int64 begin = GetTickCount();
    f();
    return GetTickCount() - begin;
}