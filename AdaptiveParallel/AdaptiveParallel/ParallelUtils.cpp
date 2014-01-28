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
    if (!_read)
    {
        std::vector<double> times(_technologies.size(),-1.0);
        for (std::size_t i=0; i<_technologies.size(); ++i)
        {
            times[i] = _technologies[i]->Run(f, iStart, iEnd);
            std::cout << _technologies[i]->GetName().c_str() << ": " << times[i] << std::endl;
        }
        _statistics->Add(times); 
    }
    else
    {
        const std::vector<double>& times = _statistics->_times[_index++];
        auto best = std::min_element(times.begin(), times.end()) - times.begin();
        auto time = _technologies[best]->Run(f, iStart, iEnd);

        std::cout << _technologies[best]->GetName().c_str() << ": " << time << std::endl;
    }
}

ParallelUtils::ParallelUtils() :_read(false),_index(0)
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

ParallelUtils::ParallelUtils( const std::vector<Technology>& iTechnologies )
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

bool ParallelUtils::FileExists( std::string fname )
{
    return std::ifstream(fname, std::ios::in | std::ios::_Nocreate) != NULL;
}

void ParallelUtils::ReadSettingsFromFile( std::string fname /*= "Settings.ini"*/ )
{
    if (!FileExists(fname))
    {
        _read = false;
        return;
    }

    std::ifstream input(fname);

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
            std::vector<double> times;

            std::istringstream istr(s);
            std::copy(std::istream_iterator<double>(istr), std::istream_iterator<double>(), back_inserter(times));
            _statistics->Add(times);

            _read = true;
        }
    }
}

void ParallelUtils::WriteToFile( std::string fname /*= "Settings.ini"*/ ) const
{
    std::ofstream output(fname, std::ios_base::app);

    for (std::size_t i=0; i<_technologies.size(); ++i)
    {
        output << _technologies[i]->GetName().c_str() << std::endl;
    }

    for (std::size_t i=0;i<_statistics->_times.size();++i)
    {
        const std::vector<double>& times = _statistics->_times[i];

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

void ParallelTimes::Add( const std::vector<double>& times )
{
    _times.push_back(times);
}
