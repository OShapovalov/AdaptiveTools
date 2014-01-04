#include "ParallelUtils.h"
#include <omp.h>
#include <fstream>

#include <iostream>
#include "ParallelTechnology.h"
#include "OmpTechnology.h"
#include "CilkTechnology.h"
#include "SerialTechnology.h"

void ParallelUtils::RunInParallel( std::function<void (int)> f, int iStart, int iEnd )
{    
    std::vector<double> times(_technologies.size(),-1.0);

    //int index = FindNearest(f, iEnd-iStart);
    //if (index >=0 )
    //{
    //    times[index] = _technologies[index].Run(f, iStart, iEnd);
    //    ParallelTimes pTimes(f, iEnd-iStart, times);
    //    _timesForFuction.push_back(pTimes);
    //}

    double minTime = 1e6;
    int index = -1;

    for (std::size_t i=0; i<_technologies.size(); ++i)
    {
        times[i] = _technologies[i]->Run(f, iStart, iEnd);
        if (times[i] < minTime)
        {
            minTime = times[i];
            index = (int)i;
        }
        std::cout << times[i] << std::endl;
    }

    
    
    //ParallelTimes pTimes(f, iEnd-iStart, times);
    //_timesForFuction.push_back(pTimes);

    

    //double serialTime = RunSerial(f, iStart, iEnd);
    //double ompTime = RunOMP(f, iStart, iEnd);
    //_statistics.push_back(ParallelInfo(f, iStart, iEnd, ompTime, serialTime));
}

ParallelUtils::ParallelUtils() :_counter(0)
{
    _technologies.push_back(std::make_shared<SerialTechnology>(SerialTechnology()));
    _technologies.push_back(std::make_shared<OmpTechnology>(OmpTechnology()));
#ifdef CILK
    _technologies.push_back(std::make_shared<CilkTechnology>(CilkTechnology()));
#endif
}

void ParallelUtils::Synchronize( int index )
{
    _technologies[index]->Synchronize();
}

//double ParallelUtils::GetTimeForFunction( std::function<void (int)> f )
//{
//    double timeStart = omp_get_wtime();
//    const int N = 10;
//    for (int i = 0; i < 10; ++i)
//    {
//        f(i);
//    }
//
//    double time = (omp_get_wtime() - timeStart)/10.0;
//    return time;
//}

//void ParallelUtils::OutputStatisticsToFile()
//{
//    std::ofstream file("Statistics.txt");
//    for (std::size_t i=0; i < _statistics.size(); ++i)
//    {
//        OutputInfoToFile(file, _statistics[i]);
//    }
//    file.close();
//}
//
//void ParallelUtils::OutputInfoToFile( const std::ofstream& iFile, const ParallelInfo& iInfo )
//{
//    iFile << "Function" << iInfo._f << std::endl;
//    iFile << "" << iInfo._start << std::endl;
//    iFile << "" << iInfo._end << std::endl;
//    iFile << "" << iInfo._serialTime << std::endl;
//    iFile << "" << iInfo._ompTime << std::endl;
//}

//int ParallelUtils::FindNearest( std::function<void (int ) > f, int iSize )
//{
//    for (std::size_t i = 0; i < _timesForFuction.size(); ++i)
//    {
//        if (/*std::get<0>(_timesForFuction[i]) == f && */std::abs(iSize - std::get<1>(_timesForFuction[i])) < 0.5*iSize )
//            return (int)i;
//    }
//
//    return -1;
//}


ParallelInfo::ParallelInfo( std::function<void (int)> f, int iStart, int iEnd ) :_f(f), _start(iStart), _end(iEnd)
{
    
}

