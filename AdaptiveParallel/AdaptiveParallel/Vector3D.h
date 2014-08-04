#pragma once
#include "Vector2D.h"

class ptlVector3D
{
public:

	ptlVector3D():_x(0),_y(0),_z(0){};
	ptlVector3D(double x, double y, double z):_x(x),_y(y),_z(z){};
	ptlVector3D(const PTLVector2D& iVec2D):_x(iVec2D[0]),_y(iVec2D[1]),_z(0){};
	~ptlVector3D(){};

    ptlVector3D & operator = (ptlVector3D const & iToCopy)
    {
        _x = iToCopy[0];
        _y = iToCopy[1];
        _z = iToCopy[2];
        return *this;
    }

	bool IsEqual(const ptlVector3D& iVec3D, double iTolerance);

	const ptlVector3D operator+(const ptlVector3D& rv) const {
		return ptlVector3D(_x + rv._x, _y + rv._y, _z + rv._z);
	}

	const ptlVector3D operator-(const ptlVector3D& rv) const {
		return ptlVector3D(_x - rv._x, _y - rv._y, _z - rv._z);
	}

	const ptlVector3D operator/(double rv) const {
		return ptlVector3D(_x / rv, _y / rv, _z / rv);
	}

	const ptlVector3D operator*(double rv) const {
		return ptlVector3D(_x * rv, _y * rv, _z * rv);
	}

	double operator*(const ptlVector3D& rv) const {
		return _x * rv._x + _y * rv._y + _z * rv._z;
	}

	//унарный +
	friend const ptlVector3D& operator+(const ptlVector3D& i)
	{
		return i;
	}

	//унарный -
	friend const ptlVector3D operator-(const ptlVector3D& i)
	{
		return ptlVector3D(-i._x,-i._y,-i._z);
	}

	double &operator[] (int index);

	double operator[] (int index) const;

private:
	double _x;
	double _y;
	double _z;
};