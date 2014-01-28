#include "PPLTechnology.h"

#ifdef TPL_PPL

#include <ppl.h>
#include "AbstractParallel.h"

double PPLTechnology::Run( std::function<void (int)> f, int iStart, int iEnd )
{
    double timeStart = AbstractParallel::GetTime();

    Concurrency::parallel_for(iStart, iEnd, f);

    double time = (AbstractParallel::GetTime() - timeStart);

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

#endif