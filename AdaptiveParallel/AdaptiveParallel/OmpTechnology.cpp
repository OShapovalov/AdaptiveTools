#include "OmpTechnology.h"
#include "AbstractParallel.h"

double OmpTechnology::Run( IAloneFunction f, int iStart, int iEnd )
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

void OmpTechnology::RunSpawn( IVoidFunction f )
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