#include "SerialTechnology.h"
#include <omp.h>

double SerialTechnology::Run( std::function<void (int)> f, int iStart, int iEnd )
{
    double timeStart = omp_get_wtime();
    int i;

    for (i = iStart; i < iEnd; ++i)
    {
        f(i);
    }
    double time = (omp_get_wtime() - timeStart);

    return time;
}

void SerialTechnology::RunSpawn( std::function<void (void)> f)
{
    f();
}