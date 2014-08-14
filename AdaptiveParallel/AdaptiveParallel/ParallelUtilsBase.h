#pragma once
#include "ParallelUtils.h"
#include <algorithm>
#include <memory>
#include <map>

enum Strategy
{
    SerialStrategy,
    BestTechnologyStrategy,
    OnlyBestStrategy
};

class ParallelUtilsBase : std::enable_shared_from_this<ParallelUtilsBase>
{
public:

    ParallelUtilsBase(Strategy iStrategy = OnlyBestStrategy):_strategy(iStrategy),_learning(false),_setTechnology(false){};

    void SetStrategy(Strategy iStrategy);

    Strategy GetStrategy() const;

    void SetTechnology(Technology iTechnology);

    void SetTechnologies(const std::vector<Technology>& iTechnologies);

    void TurnOffParallel();

    void SetLearning(bool learning);

    //const ParallelUtilsPtr& AddNewParUtils(std::string iTag /*= "Settings.xml"*/);

    const ParallelUtilsPtr& AddNewParUtils(const std::vector<Technology>& iTechnologies, std::string iTag /*= "Settings.xml"*/);

    //void RunInParallel( IAloneFunction f, int iStart, int iEnd, const std::string& iName);

    //void RunInParallel( IAloneFunction f, int iStart, int iEnd, 
    //    const std::vector< std::pair<IManyFunction , bool > >& iAddImpl, const std::string& iName);

    ParallelUtilsPtr GetParUtilsByName(const std::string& iName);

    bool IsTechnologySetted()
    {
        return _setTechnology;
    }

    const std::vector<Technology>& GetTechnologies()
    {
        return _pTechnologies;
    }

    const ParallelTechnologyPtr& GetBestTechnology()
    {
        if (!_best) // ищем лучшую по общему времени технологию, общую для всех утилз 
        {
            std::map<ParallelTechnologyPtr, double> times;
            std::map<ParallelTechnologyPtr, int> counts;

            for (std::size_t i=0; i<_allUtils.size(); ++i)
            for (std::size_t k=0;k<_allUtils[i]->_technologies.size();++k)
            {
                times[_allUtils[i]->_technologies[k]] = 0.0;
                counts[_allUtils[i]->_technologies[k]] = 0;
            }

            std::size_t maxCount = 0;

            for (std::size_t i=0; i<_allUtils.size(); ++i)
            {
                maxCount += _allUtils[i]->_sortedTimes.size();

                for (std::size_t k=0;k<_allUtils[i]->_sortedTimes.size();++k)
                {
                    for (std::size_t l=0;l<_allUtils[i]->_sortedTimes[k].second.size();++l)
                    {
                        times[_allUtils[i]->_technologies[l]] += _allUtils[i]->_sortedTimes[k].second[l];
                        counts[_allUtils[i]->_technologies[l]] ++;
                    }
                }
            }
            double minTime = 1e23;
            for (auto it=times.begin();it!=times.end();++it)
            {
                if (counts[(*it).first] != maxCount)
                    continue;
                if ((*it).second < minTime)
                {
                    minTime = (*it).second;
                    _best = (*it).first;
                }
            }
        }
        return _best;
    }

private:

    std::vector<Technology> _pTechnologies;

    bool _setTechnology;

    std::vector<ParallelUtilsPtr> _allUtils;

    Strategy _strategy;

    ParallelTechnologyPtr _best;

    bool _learning;
};