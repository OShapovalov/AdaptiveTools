#include "DoubleDoubleFunctions.h"
#include <Windows.h>

double Tf2::Evaluate( const double& iParam ) const
{
	//Sleep(5);
	return iParam*iParam;
}

void Tf2::GetIntervals(std::vector<TplInterval>& oIntervals) const
{
	oIntervals.clear();
	oIntervals.push_back(TplInterval(0,1));
}


double Tfsin::Evaluate( const double& iParam ) const
{
    int N = 300;
    float* a = new float[N*N];
    float* b = new float[N*N];
    float* res = new float[N*N];

    std::fill(a,&a[N*N], 1.0f);
    std::fill(b,&b[N*N], 2.0f);
    std::fill(res,&res[N*N], 0.0f);

    for (int i=0;i<N;++i)
    {
        for(int j=0;j<N;j++)
        {
            res[i*N+j]=0;
            for (int k=0;k<N;k++) 
                res[i*N+j]+=a[i*N+k]*b[k*N+j];
        }
    }
	//Sleep(5); 
	return std::abs(sin(31*iParam));
}

void Tfsin::GetIntervals(std::vector<TplInterval>& oIntervals) const
{
	oIntervals.clear();
	oIntervals.push_back(TplInterval(0,1));
}
