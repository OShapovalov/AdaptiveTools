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

    //ParallelUtils(std::string iTag /*= "Settings.xml"*/);

    ParallelUtils(const std::vector<Technology>& iTechnologies, std::string iTag /*= "Settings.xml"*/);

    ~ParallelUtils();

    void RunInParallel( IAloneFunction f, int iStart, int iEnd );

    void RunInParallel( IAloneFunction f, int iStart, int iEnd, 
        const std::vector< std::pair<IManyFunction , bool > >& iAddImpl);

    void RunInParallel( IAloneFunction f, int iStart, int iEnd, 
        const std::vector<Technology>& iTechnologies);

    void RunInParallel( IAloneFunction f, int iStart, int iEnd, 
        const std::vector< std::pair<IManyFunction , bool > >& iAddImpl, 
        const std::vector<Technology>& iTechnologies);

    // для сравнения двух алгоритмов
    void CompareRealizations( const std::vector< IVoidFunction >& iFunctions);

    static bool FileExists(const std::string& iName);

    void ReadSettingsFromFile();

    void WriteToFile();

    ParallelTechnologyPtr GetTechnologyByName(const std::string& iName);

    ParallelTechnologyPtr GetTechnologyByEnum(const Technology& iName);

    const std::string& GetName()
    {
        return _tag;
    }

    void SetLearning(bool iLearning)
    {
        _learning = iLearning;
    }

    bool GetLearning()
    {
        return _learning;
    }

    void SetAutoLearning(bool iLearning)
    {
        _autoLearning = iLearning;
    }

    bool GetAutoLearning()
    {
        return _autoLearning;
    }

protected:

    void RunInParallel( IAloneFunction f, int iStart, int iEnd, 
        const std::vector< std::pair<IManyFunction , bool > >& iAddImpl, 
        const std::vector<ParallelTechnologyPtr>& iTechnologies );

    void SynchronizeTechnologies();

    void ReadCommonStatistics();

    void ReadSingleStatistics();

    void WriteCommonStatistics() const;

    void WriteSingleStatistics() const;

    void ConvertCommonStatisticsToSingle();

    bool _learning;
    //int _index;

    //bool _readButNew;

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


