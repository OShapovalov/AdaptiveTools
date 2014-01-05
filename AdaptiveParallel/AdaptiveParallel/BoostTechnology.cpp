//#ifdef BOOST

#include "BoostTechnology.h"
#include <boost/thread.hpp>
#include <omp.h>

using namespace boost;
using namespace boost::this_thread;

double BoostTechnology::Run( std::function<void (int)> f, int iStart, int iEnd )
{
    double timeStart = omp_get_wtime();

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

    double time = (omp_get_wtime() - timeStart);

    return time;
}

void BoostTechnology::RunSpawn( std::function<void (void)> f )
{
    //#pragma omp task
    {
        f();
    }
}

void BoostTechnology::Synchronize()
{
    //#pragma omp taskwait
}

//#endif