
#include "PPLTechnology.h"

#ifdef PTL_PPL

#include <ppl.h>
#include "AbstractParallel.h"

Concurrency::structured_task_group tasks;

double PPLTechnology::Run( IAloneFunction f, int iStart, int iEnd )
{
    double timeStart = AbstractParallel::GetTime();

    Concurrency::parallel_for(iStart, iEnd, f);

    double time = (AbstractParallel::GetTime() - timeStart);

    return time;
}

void PPLTechnology::RunSpawn( IVoidFunction f )
{
    auto task = Concurrency::make_task(f);
    tasks.run(task);
}

void PPLTechnology::Synchronize()
{
    tasks.wait();
    tasks.cancel();
}

#endif