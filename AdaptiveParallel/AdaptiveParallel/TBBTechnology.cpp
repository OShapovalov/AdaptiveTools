#include "TBBTechnology.h"

#include "tbb/task_group.h"

#ifdef PTL_TBB

#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"
#include "AbstractParallel.h"

tbb::task_group g;

double TBBTechnology::Run( IAloneFunction f, int iStart, int iEnd )
{
    double timeStart = AbstractParallel::GetTime();

    tbb::parallel_for<int>( iStart, iEnd, 1, f );

    double time = (AbstractParallel::GetTime() - timeStart);

    return time;
}

void TBBTechnology::RunSpawn( IVoidFunction f )
{
    g.run( [&] { f(); } );
}

void TBBTechnology::Synchronize()
{
    g.wait();
}

#endif