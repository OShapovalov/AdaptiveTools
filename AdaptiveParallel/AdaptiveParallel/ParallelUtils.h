#pragma once
#include <functional>
#include <vector>
#include <map>
#include <tuple>
#include "ParallelTechnology.h"
#include "forward.h"

class ParallelInfo
{
public:
    ParallelInfo(std::function<void (int)> f, int iStart, int iEnd);

private:

    std::function<void (int)> _f;
    int _start;
    int _end;
};

class ParallelTimes
{
public:
    ParallelTimes(){};

    void Add(ParallelInfoPtr /*pInfo*/, const std::vector<double>& times)
    {
        _times.push_back(times);
    }

private:

    std::vector<std::vector<double>> _times;
};

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
    ParallelTimesPtr _statistics;
    std::vector<ParallelTechnologyPtr> _technologies;
    //std::vector< ParallelTimes > _timesForFuction; 
};


