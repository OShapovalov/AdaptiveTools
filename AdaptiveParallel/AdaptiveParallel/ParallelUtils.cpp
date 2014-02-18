#include "ParallelUtils.h"

#include "ParallelTechnology.h"
#include "OmpTechnology.h"
#include "SerialTechnology.h"

#ifdef TPL_CILK
#include "CilkTechnology.h"
#endif

#ifdef TPL_PPL
#include "PPLTechnology.h"
#endif

#ifdef TPL_TBB
#include "TBBTechnology.h"
#endif

#ifdef TPL_BOOST
#include "BoostTechnology.h"
#endif

#include <iterator>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <iostream>

void ParallelUtils::RunInParallel( std::function<void (int)> f, int iStart, int iEnd )
{    
    return RunInParallel(f, iStart, iEnd, _technologies);
}

void ParallelUtils::RunInParallel( std::function<void (int)> f, int iStart, int iEnd, const std::vector< std::pair< std::function<double (int,int)>, bool > >& iAddImpl )
{
    return RunInParallel(f, iStart, iEnd, iAddImpl, _technologies);
}

void ParallelUtils::RunInParallel( std::function<void (int)> f, int iStart, int iEnd, const std::vector<ParallelTechnologyPtr>& iTechnologies )
{
    return RunInParallel(f, iStart, iEnd, std::vector<std::pair<std::function<double (int,int)> , bool>>(), iTechnologies);
}

void ParallelUtils::RunInParallel( std::function<void (int)> f, int iStart, int iEnd, const std::vector< std::pair<std::function<double (int,int)> , bool > >& iAddImpl, const std::vector<ParallelTechnologyPtr>& iTechnologies )
{
    if (!_read)
    {
        std::vector<double> times(iTechnologies.size()+iAddImpl.size(),-1.0);
        for (std::size_t i=0; i<iTechnologies.size(); ++i)
        {
            times[i] = iTechnologies[i]->Run(f, iStart, iEnd);
            std::cout << iTechnologies[i]->GetName().c_str() << ": " << times[i] << std::endl;
        }

        for (std::size_t i=0;i<iAddImpl.size();++i)
        {
            times[iTechnologies.size() + i] = iAddImpl[i].first(iStart, iEnd);
            std::cout << "AddImpl" << i << ": " << times[iTechnologies.size() + i] << std::endl;
        }

        _statistics->Add(iEnd-iStart, times); 
    }
    else if (!_readButNew && _index<_statistics->_times.size() && (iEnd-iStart ==  _statistics->_times[_index].first) )
    {
        const std::vector<double>& times = _statistics->_times[_index++].second;
        auto best = std::min_element(times.begin(), times.end()) - times.begin();
        auto time = ( best < (int)iTechnologies.size() ) ?  
            iTechnologies[best]->Run(f, iStart, iEnd) : 
        iAddImpl[best-iTechnologies.size()].first(iStart, iEnd);

        if (best < (int)iTechnologies.size()) 
            std::cout << iTechnologies[best]->GetName().c_str() << ": " << time << std::endl;
        else
            std::cout << "AddImpl" << best-_technologies.size() << ": " << time << std::endl;
    }

    if (!_readButNew)
    {
        if ( _index>=_statistics->_times.size() || (iEnd-iStart !=  _statistics->_times[_index].first) )
            _readButNew = true;
    }

    if (_readButNew)
    {
        auto index = std::lower_bound(_sortedTimes.begin(),_sortedTimes.end(), iEnd - iStart,
            [](const std::pair<int,std::vector<double>>& p1, int p2)
        { return p1.first < p2; }
        ) - _sortedTimes.begin();

        double percent = (iEnd-iStart - _sortedTimes[index-1].first) / (_sortedTimes[index].first - _sortedTimes[index-1].first);

        std::vector<double> times(_technologies.size());
        for (std::size_t i=0;i<times.size();++i)
        {
            times[i] = _sortedTimes[index-1].second[i]*(1 - percent) + _sortedTimes[index].second[i]*percent;
        }

        auto best = std::min_element(times.begin(), times.end()) - times.begin();
        auto time = ( best < (int)iTechnologies.size() ) ?  
            iTechnologies[best]->Run(f, iStart, iEnd) : 
        iAddImpl[best-iTechnologies.size()].first(iStart, iEnd);

        if (best < (int)iTechnologies.size()) 
            std::cout << iTechnologies[best]->GetName().c_str() << ": " << time << std::endl;
        else
            std::cout << "AddImpl" << best-iTechnologies.size() << ": " << time << std::endl;
    }
}

ParallelUtils::ParallelUtils(std::string iTag /*= "Settings.ini"*/) :_read(false),_index(0),_tag(iTag),_readButNew(false)
{
    if (!TryRead())
    {
        _statistics = std::make_shared<ParallelTimes>();

        _technologies.push_back(std::make_shared<SerialTechnology>());
        _technologies.push_back(std::make_shared<OmpTechnology>());

#ifdef TPL_PPL
        _technologies.push_back(std::make_shared<PPLTechnology>());
#endif

#ifdef TPL_TBB
        _technologies.push_back(std::make_shared<TBBTechnology>());
#endif

#ifdef TPL_BOOST
        _technologies.push_back(std::make_shared<BoostTechnology>());
#endif

#ifdef TPL_CILK
        _technologies.push_back(std::make_shared<CilkTechnology>());
#endif
    }
}

ParallelUtils::ParallelUtils( const std::vector<Technology>& iTechnologies, std::string iTag /*= "Settings.ini"*/) :
                                _read(false),_index(0),_tag(iTag),_readButNew(false)
{
    if (!TryRead())
    {
        _statistics = std::make_shared<ParallelTimes>();

        for (std::size_t i=0; i<iTechnologies.size(); ++i)
        {
            _technologies.push_back( GetTechnologyByEnum(iTechnologies[i]) );
        }
    }
}

void ParallelUtils::Synchronize( int index )
{
    _technologies[index]->Synchronize();
}

bool ParallelUtils::FileExists() const
{
    return std::ifstream(_tag, std::ios::in | std::ios::_Nocreate) != NULL;
}

void ParallelUtils::ReadSettingsFromFile()
{
    if (!FileExists())
    {
        _read = false;
        return;
    }

    std::ifstream input(_tag);

    bool endTech = false;
    _statistics = std::make_shared<ParallelTimes>();

    while (! input.eof())
    {
        std::string s;
        std::getline(input,s);
        if (s.empty())
            break;

        if (!endTech)
        {
            auto tech = GetTechnologyByName(s);        
            if (tech == nullptr)
                endTech = true;
            else
                _technologies.push_back(tech);
        }

        if (endTech)
        {
            int N;
            std::istringstream ( s ) >> N;

            std::getline(input,s);
            if (s.empty())
                break;

            std::vector<double> times;

            std::istringstream istr(s);
            std::copy(std::istream_iterator<double>(istr), std::istream_iterator<double>(), back_inserter(times));
            _statistics->Add(N, times);

            _read = true;
        }
    }

    _sortedTimes = _statistics->_times;
    std::sort(_sortedTimes.begin(), _sortedTimes.end(),
        [](std::pair<int,std::vector<double>>& p1, std::pair<int,std::vector<double>>& p2)
            {return p1.first < p2.first;}
        );

    for (std::size_t i=1; i<_sortedTimes.size();++i)
    {
        if (_sortedTimes[i].first == _sortedTimes[i-1].first)
        {
            for (std::size_t k=0;k<_sortedTimes[i-1].second.size();++k)
            {
                _sortedTimes[i-1].second[k] = (_sortedTimes[i-1].second[k]+_sortedTimes[i].second[k])*0.5;
            }
            _sortedTimes.erase(_sortedTimes.begin()+i);
            --i;
        }
    }
}

void ParallelUtils::WriteToFile() const
{
    std::ofstream output(_tag, std::ios_base::app);

    for (std::size_t i=0; i<_technologies.size(); ++i)
    {
        output << _technologies[i]->GetName().c_str() << std::endl;
    }

    for (std::size_t i=0;i<_statistics->_times.size();++i)
    {
        output << _statistics->_times[i].first << std::endl;

        const std::vector<double>& times = _statistics->_times[i].second;

        for (std::size_t k=0; k<times.size(); ++k)
        {
            output << times[k] << " ";
        }
        output << std::endl;
    }

    output.close();
}

bool ParallelUtils::TryRead()
{
    ReadSettingsFromFile();
    return _read;
}

ParallelTechnologyPtr ParallelUtils::GetTechnologyByName( const std::string& iName )
{
    if (iName == "OpenMP")
        return std::make_shared<OmpTechnology>();

    #ifdef TPL_PPL
    if (iName == "PPL")
        return std::make_shared<PPLTechnology>();
#endif

    #ifdef TPL_TBB
    if (iName == "TBB")
        return std::make_shared<TBBTechnology>();
    #endif

    #ifdef TPL_CILK
    if (iName == "Cilk Plus")
        return std::make_shared<CilkTechnology>();
    #endif

    #ifdef TPL_BOOST
    if (iName == "Boost Threads")
        return std::make_shared<BoostTechnology>();
    #endif

    if (iName == "Serial")
        return std::make_shared<SerialTechnology>();

    return nullptr;
}

ParallelUtils::~ParallelUtils()
{
    if (!_read && !_statistics->_times.empty())
    {
        WriteToFile();
    }
}

ParallelTechnologyPtr ParallelUtils::GetTechnologyByEnum( const Technology& iName )
{
    if (iName == OpenMP)
        return std::make_shared<OmpTechnology>();
#ifdef TPL_PPL
    if (iName == PPL)
        return std::make_shared<PPLTechnology>();
#endif

    #ifdef TPL_TBB
    if (iName == TBB)
        return std::make_shared<TBBTechnology>();
    #endif
    #ifdef TPL_CILK
    if (iName == CilkPlus)
        return std::make_shared<CilkTechnology>();
    #endif
    #ifdef TPL_BOOST
    if (iName == BoostThreads)
        return std::make_shared<BoostTechnology>();
    #endif
    if (iName == Serial)
        return std::make_shared<SerialTechnology>();

    return nullptr;
}

void ParallelTimes::Add(int N, const std::vector<double>& times )
{
    _times.push_back(std::make_pair(N,times));
}
