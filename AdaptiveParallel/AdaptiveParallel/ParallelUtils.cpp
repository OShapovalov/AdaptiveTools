#include "ParallelUtils.h"
#include <omp.h>
#include <fstream>

#include <iostream>
#include "ParallelTechnology.h"
#include "OmpTechnology.h"
#include "CilkTechnology.h"
#include "SerialTechnology.h"
#include "PPLTechnology.h"
#include "TBBTechnology.h"
#include "BoostTechnology.h"
#include <string>
#include <iterator>
#include "Statistics.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>

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
        _technologies.push_back(std::make_shared<PPLTechnology>());
        _technologies.push_back(std::make_shared<TBBTechnology>());
        _technologies.push_back(std::make_shared<BoostTechnology>());
        _technologies.push_back(std::make_shared<CilkTechnology>());
    }
}

void ParallelUtils::Synchronize( int index )
{
    _technologies[index]->Synchronize();
}

//double ParallelUtils::GetTimeForFunction( std::function<void (int)> f )
//{
//    double timeStart = AbstractParallel::GetTime();
//    const int N = 10;
//    for (int i = 0; i < 10; ++i)
//    {
//        f(i);
//    }
//
//    double time = (AbstractParallel::GetTime() - timeStart)/10.0;
//    return time;
//}

//void ParallelUtils::OutputStatisticsToFile()
//{
//    std::ofstream file("Statistics.txt");
//    for (std::size_t i=0; i < _statistics.size(); ++i)
//    {
//        OutputInfoToFile(file, _statistics[i]);
//    }
//    file.close();
//}
//
//void ParallelUtils::OutputInfoToFile( const std::ofstream& iFile, const ParallelInfo& iInfo )
//{
//    iFile << "Function" << iInfo._f << std::endl;
//    iFile << "" << iInfo._start << std::endl;
//    iFile << "" << iInfo._end << std::endl;
//    iFile << "" << iInfo._serialTime << std::endl;
//    iFile << "" << iInfo._ompTime << std::endl;
//}

//int ParallelUtils::FindNearest( std::function<void (int ) > f, int iSize )
//{
//    for (std::size_t i = 0; i < _timesForFuction.size(); ++i)
//    {
//        if (/*std::get<0>(_timesForFuction[i]) == f && */std::abs(iSize - std::get<1>(_timesForFuction[i])) < 0.5*iSize )
//            return (int)i;
//    }
//
//    return -1;
//}


//ParallelInfo::ParallelInfo( std::function<void (int)> f, int iStart, int iEnd ) :_f(f), _start(iStart), _end(iEnd)
//{
//    
//}

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
    if (iName == "PPL")
        return std::make_shared<PPLTechnology>();
    if (iName == "TBB")
        return std::make_shared<TBBTechnology>();
    if (iName == "Cilk Plus")
        return std::make_shared<CilkTechnology>();
    if (iName == "Boost Threads")
        return std::make_shared<BoostTechnology>();
    if (iName == "Serial")
        return std::make_shared<SerialTechnology>();

    return nullptr;
}

ParallelUtils::~ParallelUtils()
{
    WriteToFile();
}
