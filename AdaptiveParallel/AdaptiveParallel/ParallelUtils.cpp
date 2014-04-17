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
#include "AbstractParallel.h"
#include <memory>
#include "ParallelUtilsBase.h"
#include "pugixml.hpp"

void ParallelUtils::RunInParallel( std::function<void (int)> f, int iStart, int iEnd )
{    
    if (_baseUtils->IsTechnologySetted())
    {
        return RunInParallel(f, iStart, iEnd, _baseUtils->GetTechnologies());
    }

    if (_technologiesName.size() != _technologies.size())
        SynchronizeTechnologies();

    return RunInParallel(f, iStart, iEnd, _technologiesName);
}

void ParallelUtils::RunInParallel( std::function<void (int)> f, int iStart, int iEnd, const std::vector< std::pair< std::function<double (int,int)>, bool > >& iAddImpl )
{
    if (_baseUtils->IsTechnologySetted())
    {
        return RunInParallel(f, iStart, iEnd, _baseUtils->GetTechnologies());
    }

    if (_technologiesName.size() != _technologies.size())
        SynchronizeTechnologies();

    return RunInParallel(f, iStart, iEnd, iAddImpl, _technologiesName);
}

void ParallelUtils::SynchronizeTechnologies()
{
    _technologiesName.resize(_technologies.size());
    for (std::size_t i=0;i<_technologies.size();++i)
    {
        _technologiesName[i] = _technologies[i]->GetEnum();
    }
}

void ParallelUtils::RunInParallel( std::function<void (int)> f, int iStart, int iEnd, const std::vector<Technology>& iTechnologies )
{
    if (_baseUtils->IsTechnologySetted())
    {
        return RunInParallel(f, iStart, iEnd, _baseUtils->GetTechnologies());
    }
    return RunInParallel(f, iStart, iEnd, std::vector<std::pair<std::function<double (int,int)> , bool>>(), iTechnologies);
}

void ParallelUtils::RunInParallel( std::function<void (int)> f, int iStart, int iEnd, 
    const std::vector< std::pair<std::function<double (int,int)> , bool > >& iAddImpl, 
    const std::vector<Technology>& iTechnologies )
{
    if (_baseUtils->IsTechnologySetted())
    {
        return RunInParallel(f, iStart, iEnd, _baseUtils->GetTechnologies());
    }

    std::vector<ParallelTechnologyPtr> ptechs(iTechnologies.size());
    for (std::size_t i=0;i<ptechs.size();++i)
    {
        ptechs[i] = GetTechnologyByEnum(iTechnologies[i]);
    }

    return RunInParallel(f, iStart, iEnd, iAddImpl, ptechs);
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
    else if (!_readButNew && 
                _index<_statistics->_times.size() && 
                (iEnd-iStart ==  _statistics->_times[_index].first) 
                && iTechnologies.size()+iAddImpl.size() == _statistics->_times.front().second.size())
    {
        switch (_baseUtils->GetStrategy())
        {
        case SerialStrategy:
            for (int i=iStart;i<iEnd;++i)
            {
                f(i);
            }
        	break;
        case BestTechnologyStrategy:
            _baseUtils->GetBestTechnology()->Run(f, iStart, iEnd);
            break;
        case OnlyBestStrategy:      
            const std::vector<double>& times = _statistics->_times[_index++].second;
            auto best = std::min_element(times.begin(), times.end()) - times.begin();
            auto time = ( best < (int)iTechnologies.size() ) ?  
                iTechnologies[best]->Run(f, iStart, iEnd) : 
            iAddImpl[best-iTechnologies.size()].first(iStart, iEnd);

            if (best < (int)iTechnologies.size()) 
                std::cout << iTechnologies[best]->GetName().c_str() << ": " << time << std::endl;
            else
                std::cout << "AddImpl" << best-_technologies.size() << ": " << time << std::endl;
            break;
        }

    }

    if (!_readButNew)
    {
        if ( _index>=_statistics->_times.size() || (iEnd-iStart !=  _statistics->_times[_index].first) )
            _readButNew = true;
    }

    if (_readButNew)
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

        //auto index = std::lower_bound(_sortedTimes.begin(),_sortedTimes.end(), iEnd - iStart,
        //    [](const std::pair<int,std::vector<double>>& p1, int p2)
        //{ return p1.first < p2; }
        //) - _sortedTimes.begin();

        //double percent = (iEnd-iStart - _sortedTimes[index-1].first) / (_sortedTimes[index].first - _sortedTimes[index-1].first);

        //std::vector<double> times(_technologies.size());
        //for (std::size_t i=0;i<times.size();++i)
        //{
        //    times[i] = _sortedTimes[index-1].second[i]*(1 - percent) + _sortedTimes[index].second[i]*percent;
        //}

        //auto best = std::min_element(times.begin(), times.end()) - times.begin();
        //auto time = ( best < (int)iTechnologies.size() ) ?  
        //    iTechnologies[best]->Run(f, iStart, iEnd) : 
        //iAddImpl[best-iTechnologies.size()].first(iStart, iEnd);

        //if (best < (int)iTechnologies.size()) 
        //    std::cout << iTechnologies[best]->GetName().c_str() << ": " << time << std::endl;
        //else
        //    std::cout << "AddImpl" << best-iTechnologies.size() << ": " << time << std::endl;
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

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("test.xml");
    if (result)
        std::cout << "XML parsed without errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n\n";
    else
    {
        std::cout << "XML parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
        std::cout << "Error description: " << result.description() << "\n";
        std::cout << "Error offset: " << result.offset << " (error at [..." << (result.offset) << "]\n\n";
    }

    auto techs = doc.child("technologies");

    for (pugi::xml_node tool = techs.first_child(); tool; tool = tool.next_sibling())
    {
        auto tech = tool.first_child();
        auto techno = GetTechnologyByName(std::string(tech.value()));
        _technologies.push_back(techno);
    }

    auto statistics = doc.child("statistics");
    for (pugi::xml_node tool = statistics.first_child(); tool; tool = tool.next_sibling())
    {
        auto iter = tool.first_child();
        int iterations = atoi(iter.first_child().value());

        std::vector<double> times;

        auto times_txt = iter.next_sibling();
        for (pugi::xml_node toolInner = times_txt.first_child(); toolInner; toolInner = toolInner.next_sibling())
        {
            auto val = toolInner.first_child().value();
            //std::cout << val << std::endl;
            times.push_back(atof(val));
        }

        _statistics->Add(iterations, times);
    }

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
            {
                endTech = true;                
                //if (s.length()>7)
                //{
                //    if ()
                //}
            }
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
    pugi::xml_document doc;

    //[code_modify_add
    // add node with some name
    pugi::xml_node node = doc.append_child();
    node.set_name("technologies");

    for (std::size_t i=0;i<_technologies.size();++i)
    {
        // add description node with text child
        pugi::xml_node descr = node.append_child();
        descr.set_name("technology");
        descr.append_child(pugi::node_pcdata).set_value(_technologies[i]->GetName().c_str());
    }

    pugi::xml_node node2 = doc.append_child("statistics");
    for (std::size_t i=0;i<_statistics->_times.size();++i)
    {
        pugi::xml_node descr0 = node2.append_child();
        descr0.set_name("cycle");

        pugi::xml_node descr = descr0.append_child();
        descr.set_name("iterations");
    
        descr.append_child(pugi::node_pcdata).set_value(
            std::to_string((unsigned long long)_statistics->_times[i].first).c_str());

        const std::vector<double>& times = _statistics->_times[i].second;

        pugi::xml_node descr2 = descr0.append_child();
        descr2.set_name("times");

        for (std::size_t k=0; k<times.size(); ++k)
        {    
            std::ostringstream strs;
            strs << times[k];
            std::string str = strs.str();

            pugi::xml_node descrTime = descr2.append_child();
            descrTime.set_name("time");

            descrTime.append_child(pugi::node_pcdata).set_value(str.c_str());
        }
    }
    doc.save_file("test.xml");
    //std::ofstream output0("test.xml", std::ios_base::trunc);
    //doc.print(output0);  
    //output0.close();

    std::ofstream output(_tag, std::ios_base::trunc);

    for (std::size_t i=0; i<_technologies.size(); ++i)
    {
        output << _technologies[i]->GetName().c_str() << std::endl;
    }

    //if (!_statistics->_times.empty())
    //for (std::size_t i=0;i+_technologies.size()<_statistics->_times[0].second.size();++i)
    //{
    //    output << "AddImpl" << i << std::endl;
    //}

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
    if (_readButNew || ( !_read && !_statistics->_times.empty()) )
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

void ParallelUtils::CompareRealizations( const std::vector< std::function<void (void)> >& iFunctions )
{
    //TODO: надо что-то сделать для логичности этой функции

    std::vector<double> times( iFunctions.size() );
    for (std::size_t i=0; i<iFunctions.size(); ++i)
    {
        auto startTime = AbstractParallel::GetTime();
        iFunctions[i];
        times[i] = AbstractParallel::GetTime() - startTime;
    }
}

void ParallelTimes::Add(int N, const std::vector<double>& times )
{
    _times.push_back(std::make_pair(N,times));
}
