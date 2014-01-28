#pragma once

#include "TPLdefines.h"

#include <functional>
#include "ParallelTechnology.h"
#include "tplForward.h"
#include <vector>

class ParallelTimes
{
public:
    ParallelTimes(){};

    void Add(const std::vector<double>& times);

    std::vector<std::vector<double>> _times;
};

class ParallelUtils
{

public:

    enum Technology
    {
        Serial,
        OpenMP,
#ifdef TPL_PPL
        PPL,
#endif
#ifdef TPL_TBB
        TBB,
#endif
#ifdef TPL_CILK
        CilkPlus,
#endif
#ifdef TPL_BOOST
        BoostThreads,
#endif
    };

    ParallelUtils();

    ParallelUtils(const std::vector<Technology>& iTechnologies);

    ~ParallelUtils();

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd );  

    int RunInAnotherThread( std::function<void (void)> f ); 

    void Synchronize(int index);

    static bool FileExists(std::string fname);

    void ReadSettingsFromFile(std::string fname = "Settings.ini");

    void WriteToFile(std::string fname = "Settings.ini") const;

    bool TryRead();

    ParallelTechnologyPtr GetTechnologyByName(const std::string& iName);

    ParallelTechnologyPtr GetTechnologyByEnum(const Technology& iName);

protected:
    bool _read;
    int _index;
private:
    ParallelTimesPtr _statistics;
    std::vector<ParallelTechnologyPtr> _technologies;
};


