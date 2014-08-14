#include "TBBTechnology.h"

#ifdef PTL_TBB

#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"
#include "AbstractParallel.h"

double TBBTechnology::Run( IAloneFunction f, int iStart, int iEnd )
{
    double timeStart = AbstractParallel::GetTime();

    tbb::parallel_for<int>( iStart, iEnd, 1, f );

    double time = (AbstractParallel::GetTime() - timeStart);

    return time;
}

void TBBTechnology::RunSpawn( IVoidFunction f )
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

#endif