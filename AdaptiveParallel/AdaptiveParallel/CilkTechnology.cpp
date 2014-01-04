
#ifdef CILK

#include <cilk/cilk.h>
#include "CilkUtils.h"

double CilkTechnology::Run( std::function<void (int)> f, int iStart, int iEnd )
{
    double timeStart = omp_get_wtime();

    cilk_for(int i = iStart; i < iEnd; ++i)
    {
        f(i);
    }
    double time = (omp_get_wtime() - timeStart);

    return time;
}

void CilkTechnology::RunSpawn( std::function<void (void)> f )
{
    cilk_spawn f();
}

void CilkTechnology::Synchronize()
{
    cilk_sync;
}

#endif