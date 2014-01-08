#include "SerialTechnology.h"
#include "AbstractParallel.h"

double SerialTechnology::Run( std::function<void (int)> f, int iStart, int iEnd )
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

void SerialTechnology::RunSpawn( std::function<void (void)> f)
{
    f();
}