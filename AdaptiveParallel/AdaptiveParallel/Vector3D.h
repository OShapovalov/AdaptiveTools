#pragma once
#include "Vector2D.h"

class tplVector3D
{
public:

	tplVector3D():_x(0),_y(0),_z(0){};
	tplVector3D(double x, double y, double z):_x(x),_y(y),_z(z){};
	tplVector3D(const tplVector2D& iVec2D):_x(iVec2D[0]),_y(iVec2D[1]),_z(0){};
	~tplVector3D(){};

    tplVector3D & operator = (tplVector3D const & /*num*/)
    {
        return *this;
    }

	bool IsEqual(const tplVector3D& iVec3D, double iTolerance);

	const tplVector3D operator+(const tplVector3D& rv) const {
		return tplVector3D(_x + rv._x, _y + rv._y, _z + rv._z);
	}

	const tplVector3D operator-(const tplVector3D& rv) const {
		return tplVector3D(_x - rv._x, _y - rv._y, _z - rv._z);
	}

	const tplVector3D operator/(double rv) const {
		return tplVector3D(_x / rv, _y / rv, _z / rv);
	}

	const tplVector3D operator*(double rv) const {
		return tplVector3D(_x * rv, _y * rv, _z * rv);
	}

	double operator*(const tplVector3D& rv) const {
		return _x * rv._x + _y * rv._y + _z * rv._z;
	}

	//унарный +
	friend const tplVector3D& operator+(const tplVector3D& i)
	{
		return i;
	}

	//унарный -
	friend const tplVector3D operator-(const tplVector3D& i)
	{
		return tplVector3D(-i._x,-i._y,-i._z);
	}

	double &operator[] (int index);

	double operator[] (int index) const;

private:
	double _x;
	double _y;
	double _z;
};