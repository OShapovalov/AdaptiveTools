#include "CilkTechnology.h"

#ifdef PTL_CILK

#include <cilk/cilk.h>
//#include "CilkUtils.h"
#include <functional>

#include "AbstractParallel.h"

double CilkTechnology::Run( IAloneFunction f, int iStart, int iEnd )
{
    double timeStart = AbstractParallel::GetTime();

    cilk_for(int i = iStart; i < iEnd; ++i)
    {
        f(i);
    }
    double time = (AbstractParallel::GetTime() - timeStart);

    return time;
}

void CilkTechnology::RunSpawn( IVoidFunction f )
{
    cilk_spawn f();
}

void CilkTechnology::Synchronize()
{
    cilk_sync;
}

#endif