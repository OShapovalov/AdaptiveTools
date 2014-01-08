#include "OmpTechnology.h"
#include "AbstractParallel.h"

double OmpTechnology::Run( std::function<void (int)> f, int iStart, int iEnd )
{
    double timeStart = AbstractParallel::GetTime();
    int i;

#pragma omp parallel for 
    for (i = iStart; i < iEnd; ++i)
    {
        f(i);
    }

    double time = (AbstractParallel::GetTime() - timeStart);

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