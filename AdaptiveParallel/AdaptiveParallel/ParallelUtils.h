#pragma once
#include <functional>
#include <vector>
#include "ParallelTechnology.h"
#include "forward.h"

//class ParallelInfo
//{
//public:
//    ParallelInfo(std::function<void (int)> f, int iStart, int iEnd);
//
//private:
//
//    std::function<void (int)> _f;
//    int _start;
//    int _end;
//};

class ParallelTimes
{
public:
    ParallelTimes(){};

    void Add(const std::vector<double>& times)
    {
        _times.push_back(times);
    }

    std::vector<std::vector<double>> _times;
};

class ParallelUtils
{

public:

    ParallelUtils();

    ~ParallelUtils();

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd );  

    int RunInAnotherThread( std::function<void (void)> f ); 

    void Synchronize(int index);

    static bool FileExists(std::string fname);

    void ReadSettingsFromFile(std::string fname = "Settings.ini");

    void WriteToFile(std::string fname = "Settings.ini") const;

    bool TryRead();

    ParallelTechnologyPtr GetTechnologyByName(const std::string& iName);

protected:
    bool _read;
    int _index;
private:
    ParallelTimesPtr _statistics;
    std::vector<ParallelTechnologyPtr> _technologies;
};


