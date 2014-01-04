#pragma once
#include <functional>
#include <vector>
#include <map>
#include <tuple>
#include "ParallelTechnology.h"
#include "forward.h"

struct ParallelInfo
{
    ParallelInfo(std::function<void (int)> f, int iStart, int iEnd);

    std::function<void (int)> _f;
    int _start;
    int _end;

    std::vector<double> times;

    //double _ompTime;
    //double _serialTime;
};

typedef std::tuple< std::function<void (int)>, int , std::vector<double> > ParallelTimes;    

class ParallelUtils
{

public:

    ParallelUtils();

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd );  

    int RunInAnotherThread( std::function<void (void)> f ); 

    void Synchronize(int index);;

	//double GetTimeForFunction(std::function<void (int)> f);

	//void OutputStatisticsToFile();

    //void OutputInfoToFile(const std::ofstream& iFile, const ParallelInfo& iInfo);
    //int FindNearest( std::function<void (int ) > f, int iSize);
protected:
    int _counter;
private:
    std::vector<ParallelInfo> _statistics;
    std::vector<ParallelTechnologyPtr> _technologies;
    //std::vector< ParallelTimes > _timesForFuction; 
};


