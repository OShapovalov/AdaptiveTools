#include "SurfaceFunctions.h"

Vector3D TSomeSurface::Evaluate( const Vector2D& params ) const 
{
	double u = params[0], v = params[1];
	//return Vector3D(u,v,0);
	return Vector3D(u,v,u*sin(2*v));
}

void TSomeSurface::GetIntervals(std::vector<TplInterval>& oIntervals) const
{
	oIntervals.clear();
	oIntervals.push_back(TplInterval(0,1));
	oIntervals.push_back(TplInterval(0,1));
}