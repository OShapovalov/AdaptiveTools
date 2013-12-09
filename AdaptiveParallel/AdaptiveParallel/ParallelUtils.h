#pragma once
#include <functional>
#include <vector>
#include <map>
#include <tuple>

struct ParallelInfo
{
    ParallelInfo(std::function<void (int)> f, int iStart, int iEnd);

    std::function<void (int)> _f;
    int _start;
    int _end;

    std::vector<double> times;

    //double _ompTime;
    //double _serialTime;
};

typedef std::tuple< std::function<void (int)>, int , std::vector<double> > ParallelTimes;    

class ParallelUtils
{

public:

    ParallelUtils();

    void RunInParallel( std::function<void (int)> f, int iStart, int iEnd );  

    int RunInAnotherThread( std::function<void (void)> f ); 

    void Synchronize(int index);;

	//double GetTimeForFunction(std::function<void (int)> f);

	//void OutputStatisticsToFile();

    //void OutputInfoToFile(const std::ofstream& iFile, const ParallelInfo& iInfo);
    //int FindNearest( std::function<void (int ) > f, int iSize);
protected:
    int _counter;
private:
    std::vector<struct ParallelInfo> _statistics;
    std::vector<class ParalelTechnology> _technologies;
    //std::vector< ParallelTimes > _timesForFuction; 
};

class ParalelTechnology
{
public:

    virtual double Run(std::function<void (int)> f, int iStart, int iEnd);

    virtual void RunSpawn(std::function<void (void)> f); 

    virtual void Synchronize(){};

    virtual std::string GetName()
    {
        return "Serial";
    }
};

class OmpTechnology : public ParalelTechnology
{
public:

    virtual double Run(std::function<void (int)> f, int iStart, int iEnd) override;

    virtual void RunSpawn(std::function<void (void)> f) override; 

    virtual void Synchronize() override;;

    virtual std::string GetName() override
    {
        return "OpenMP";
    }
};

class CilkTechnology : public ParalelTechnology
{
public:

    virtual double Run(std::function<void (int)> f, int iStart, int iEnd) override;

    virtual void RunSpawn(std::function<void (void)> f) override; 

    virtual void Synchronize() override;;

    virtual std::string GetName() override
    {
        return "Cilk Plus";
    }
};