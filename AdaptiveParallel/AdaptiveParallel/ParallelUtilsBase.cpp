#include "ParallelUtilsBase.h"


void ParallelUtilsBase::SetStrategy( Strategy iStrategy )
{
    _strategy = iStrategy;
}

Strategy ParallelUtilsBase::GetStrategy() const
{
    return _strategy;
}

void ParallelUtilsBase::SetTechnology( Technology iTechnology )
{
    _setTechnology = true;
    _pTechnologies.clear();
    _pTechnologies.push_back(iTechnology);
}

void ParallelUtilsBase::SetTechnologies( const std::vector<Technology>& iTechnologies )
{
    _setTechnology = true;
    _pTechnologies = iTechnologies;
}

void ParallelUtilsBase::TurnOffParallel()
{
    _setTechnology = true;
    SetTechnology(Technology::Serial);
}

void ParallelUtilsBase::SetLearning( bool learning )
{
    _learning = learning;
}

const ParallelUtilsPtr& ParallelUtilsBase::AddNewParUtils( std::string iTag /*= "Settings.xml"*/ )
{
    _allUtils.push_back(std::make_shared<ParallelUtils>(iTag));
    _allUtils.back()->_baseUtils = this;//shared_from_this();
    return _allUtils.back();
}

const ParallelUtilsPtr& ParallelUtilsBase::AddNewParUtils( const std::vector<Technology>& iTechnologies, std::string iTag /*= "Settings.xml"*/ )
{
    _allUtils.push_back(std::make_shared<ParallelUtils>(iTechnologies,iTag));
    //auto q = std::make_shared<ParallelUtilsBase>();
    //auto q = shared_from_this();
    _allUtils.back()->_baseUtils = this;
    return _allUtils.back();
}

//void ParallelUtilsBase::RunInParallel( std::function<void (int)> f, int iStart, int iEnd, const std::string& iName )
//{
//    auto utils = GetParUtilsByName(iName);
//    if (!utils)
//        return;
//    utils->RunInParallel(f,iStart,iEnd,_pTechnologies);
//}

//void ParallelUtilsBase::RunInParallel( std::function<void (int)> f, int iStart, int iEnd, const std::vector< std::pair<std::function<double (int,int)> , bool > >& iAddImpl, const std::string& iName )
//{
//    auto utils = GetParUtilsByName(iName);
//    if (!utils)
//        return;
//    utils->RunInParallel(f,iStart,iEnd,iAddImpl,_pTechnologies);
//}

ParallelUtilsPtr ParallelUtilsBase::GetParUtilsByName( const std::string& iName )
{
    auto lambda = [iName](const ParallelUtilsPtr& pUtils) -> bool
    {
        return pUtils->GetName().compare(iName) == 0;
    };

    auto it = std::find_if(_allUtils.begin(), _allUtils.end(), lambda);
    if (it == _allUtils.end())
        return nullptr;
    else
        return *it;
}
