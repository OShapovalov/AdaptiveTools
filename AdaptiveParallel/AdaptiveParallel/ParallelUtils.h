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

    void Add(int N, const std::vector<double>& times);

    std::vector<std::pair<int,std::vector<double>>> _times;
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

    ParallelUtils(std::string iTag /*= "Settings.ini"*/);

    ParallelUtils(const std::vector<Technology>& iTechnologies, std::string iTag /*= "Settings.ini"*/);

    ~ParallelUtils();

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd );

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd, 
        const std::vector< std::function<double (int,int)> >& iAddImpl);

    int RunInAnotherThread( std::function<void (void)> f ); 

    void Synchronize(int index);

    bool FileExists() const;

    void ReadSettingsFromFile();

    void WriteToFile() const;

    bool TryRead();

    ParallelTechnologyPtr GetTechnologyByName(const std::string& iName);

    ParallelTechnologyPtr GetTechnologyByEnum(const Technology& iName);

protected:
    bool _read;
    int _index;
private:
    ParallelTimesPtr _statistics;
    std::vector<ParallelTechnologyPtr> _technologies;

    std::string _tag;
};


