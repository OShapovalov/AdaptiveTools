#include "TBBTechnology.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"
#include <omp.h>

double TBBTechnology::Run( std::function<void (int)> f, int iStart, int iEnd )
{
    double timeStart = omp_get_wtime();

    tbb::parallel_for<int>( iStart, iEnd, 1, f );

    double time = (omp_get_wtime() - timeStart);

    return time;
}

void TBBTechnology::RunSpawn( std::function<void (void)> f )
{
    //#pragma omp task
    {
        f();
    }
}

void TBBTechnology::Synchronize()
{
    //#pragma omp taskwait
}