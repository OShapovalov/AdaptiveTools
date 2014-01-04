#include "OmpTechnology.h"
#include <omp.h>

double OmpTechnology::Run( std::function<void (int)> f, int iStart, int iEnd )
{
    double timeStart = omp_get_wtime();
    int i;

#pragma omp parallel for 
    for (i = iStart; i < iEnd; ++i)
    {
        f(i);
    }

    double time = (omp_get_wtime() - timeStart);

    return time;
}

void OmpTechnology::RunSpawn( std::function<void (void)> f )
{
//#pragma omp task
    {
        f();
    }
}

void OmpTechnology::Synchronize()
{
//#pragma omp taskwait
}