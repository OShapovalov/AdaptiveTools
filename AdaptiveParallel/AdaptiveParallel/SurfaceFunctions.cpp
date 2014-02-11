#include "SurfaceFunctions.h"
#include <Windows.h>
#include <memory>

tplVector3D TSomeSurface::Evaluate( const tplVector2D& params ) const 
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

    //Sleep(100);
	double u = params[0], v = params[1];
	//return Vector3D(u,v,0);
	return tplVector3D(u,v,u*sin(2*v));
}

void TSomeSurface::GetIntervals(std::vector<TplInterval>& oIntervals) const
{
	oIntervals.clear();
	oIntervals.push_back(TplInterval(0,1));
	oIntervals.push_back(TplInterval(0,1));
}