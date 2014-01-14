#include "Vector3D.h"
#include <cmath>

double & tplVector3D::operator[] (int index) 
{
	if (index == 0) 
	{
		return _x;
	}
	else if (index == 1)
	{
		return _y;
	} else
	{
		return _z;
	}
}

double tplVector3D::operator[] (int index) const
{
	if (index == 0) 
	{
		return _x;
	}
	else if (index == 1)
	{
		return _y;
	} else
	{
		return _z;
	}
}

bool tplVector3D::IsEqual( const tplVector3D& iVec3D, double iTolerance )
{
	return ( std::abs(_x - iVec3D[0]) < iTolerance ) &&
		( std::abs(_y - iVec3D[1]) < iTolerance ) &&
		( std::abs(_z - iVec3D[2]) < iTolerance );
}
