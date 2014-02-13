#pragma once
#include "ParallelUtils.h"
#include <algorithm>

class ParallelUtilsBase
{
    void SetTechnology(ParallelUtils::Technology iTechnology)
    {
        _pTechnologies.clear();
        _pTechnologies.push_back(iTechnology);
    }

    void SetTechnologies(const std::vector<ParallelUtils::Technology>& iTechnologies)
    {
        _pTechnologies = iTechnologies;
    }

    void TurnOffParallel()
    {
        SetTechnology(ParallelUtils::Serial);
    }

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd, const std::string& iName)
    {
        auto utils = GetParUtilsByName(iName);
        if (!utils)
            return;
        utils->RunInParallel(f,iStart,iEnd);
    }

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd, 
        const std::vector< std::pair<std::function<double (int,int)> , bool > >& iAddImpl, const std::string& iName)
    {
        auto utils = GetParUtilsByName(iName);
        if (!utils)
            return;
        utils->RunInParallel(f,iStart,iEnd,iAddImpl);
    }

    ParallelUtilsPtr GetParUtilsByName(const std::string& iName)
    {
        auto lambda = [iName](const ParallelUtilsPtr& pUtils)
        {
            return pUtils->GetName().compare(iName) == 0;
        };

        auto it = std::find(_allUtils.begin(), _allUtils.end(), lambda);
        if (it == _allUtils.end())
            return nullptr;
        else
            return *it;
    }

private:

    std::vector<ParallelUtils::Technology> _pTechnologies;

    std::vector<ParallelUtilsPtr> _allUtils;
};