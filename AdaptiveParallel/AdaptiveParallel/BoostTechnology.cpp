#include "BoostTechnology.h"

#ifdef PTL_BOOST

#include <boost/thread.hpp>
#include <omp.h>
#include "AbstractParallel.h"

using namespace boost;
using namespace boost::this_thread;

std::vector< IVoidFunction > _functions;

double BoostTechnology::Run( IAloneFunction f, int iStart, int iEnd )
{
    double timeStart = AbstractParallel::GetTime();

    thread_group group;
    for(int j=iStart; j<iEnd; j++)
    {        
        //group.create_thread( [&tasks, start, taskCount]() 
        //{
        //    const size_t end = std::min(tasks.size(), start + taskCount);
        //    for (size_t i = start; i < end; ++i)
        //        tasks[i]();
        //});
        //group.create_thread(f(j));
        group.create_thread([f, j](){f(j);});
    } 
    group.join_all();

    double time = (AbstractParallel::GetTime() - timeStart);

    return time;
}

void BoostTechnology::RunSpawn( IVoidFunction f )
{
    _functions.push_back(f);
}

void BoostTechnology::Synchronize()
{
    thread_group group;
    for(std::size_t j = 0; j < _functions.size(); j++)
    {        
        group.create_thread(_functions[j]);
    } 
    group.join_all();

    _functions.clear();
}

#endif