#pragma once

#include "PTLdefines.h"

#include <functional>
#include "ParallelTechnology.h"
#include "PTLForward.h"
#include <vector>
//#include "ParallelUtilsBase.h"

class ParallelUtilsBase;

enum Technology
{
    Serial,
    OpenMP,
#ifdef PTL_PPL
    PPL,
#endif
#ifdef PTL_TBB
    TBB,
#endif
#ifdef PTL_CILK
    CilkPlus,
#endif
#ifdef PTL_BOOST
    BoostThreads,
#endif
};

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

    ParallelUtils(std::string iTag /*= "Settings.xml"*/);

    ParallelUtils(const std::vector<Technology>& iTechnologies, std::string iTag /*= "Settings.xml"*/);

    ~ParallelUtils();

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd );

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd, 
        const std::vector< std::pair<std::function<double (int,int)> , bool > >& iAddImpl);

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd, 
        const std::vector<Technology>& iTechnologies);

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd, 
        const std::vector< std::pair<std::function<double (int,int)> , bool > >& iAddImpl, 
        const std::vector<Technology>& iTechnologies);

    // для сравнения двух алгоритмов
    void CompareRealizations( const std::vector< std::function<void (void)> >& iFunctions);

    int RunInAnotherThread( std::function<void (void)> f ); 

    void Synchronize(int index);

    bool FileExists() const;

    void ReadSettingsFromFile();

    void WriteToFile() const;

    bool TryRead();

    ParallelTechnologyPtr GetTechnologyByName(const std::string& iName);

    ParallelTechnologyPtr GetTechnologyByEnum(const Technology& iName);

    const std::string& GetName()
    {
        return _tag;
    }

protected:

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd, 
        const std::vector< std::pair<std::function<double (int,int)> , bool > >& iAddImpl, 
        const std::vector<ParallelTechnologyPtr>& iTechnologies );

    void SynchronizeTechnologies();

    bool _read;
    int _index;

    bool _readButNew;

    // режим самообучения по мотивам метода отжига
    bool _autoLearning;

    // технология -> (количество итераций-время)
    std::vector< std::vector< std::pair<int, double> > > _singleStatistics;

private:
    ParallelTimesPtr _statistics;
    std::vector<ParallelTechnologyPtr> _technologies;
    std::vector<Technology> _technologiesName;

    std::string _tag;

    std::vector<std::pair<int,std::vector<double>>> _sortedTimes;

    //std::weak_ptr<ParallelUtilsBase> _baseUtils;
    ParallelUtilsBase* _baseUtils;

    friend class ParallelUtilsBase;
};


