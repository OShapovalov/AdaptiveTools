#include "SerialTechnology.h"
#include "AbstractParallel.h"

double SerialTechnology::Run( IAloneFunction f, int iStart, int iEnd )
{
    double timeStart = AbstractParallel::GetTime();
    int i;

    for (i = iStart; i < iEnd; ++i)
    {
        f(i);
    }
    double time = (AbstractParallel::GetTime() - timeStart);

    return time;
}

void SerialTechnology::RunSpawn( IVoidFunction f)
{
    f();
}