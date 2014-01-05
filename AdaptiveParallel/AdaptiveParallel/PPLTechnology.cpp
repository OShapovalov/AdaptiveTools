#include "PPLTechnology.h"
#include <ppl.h>
#include <omp.h>

double PPLTechnology::Run( std::function<void (int)> f, int iStart, int iEnd )
{
    double timeStart = omp_get_wtime();

    Concurrency::parallel_for(iStart, iEnd, f);

    double time = (omp_get_wtime() - timeStart);

    return time;
}

void PPLTechnology::RunSpawn( std::function<void (void)> f )
{
    //#pragma omp task
    {
        f();
    }
}

void PPLTechnology::Synchronize()
{
    //#pragma omp taskwait
}