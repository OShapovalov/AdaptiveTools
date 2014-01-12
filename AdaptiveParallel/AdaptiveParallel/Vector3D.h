#pragma once
#include "Vector2D.h"

class Vector3D
{
public:

	Vector3D():_x(0),_y(0),_z(0){};
	Vector3D(double x, double y, double z):_x(x),_y(y),_z(z){};
	Vector3D(const Vector2D& iVec2D):_x(iVec2D[0]),_y(iVec2D[1]),_z(0){};
	~Vector3D(){};

    Vector3D & operator = (Vector3D const & num)
    {
        return *this;
    }

	bool IsEqual(const Vector3D& iVec3D, double iTolerance);

	const Vector3D operator+(const Vector3D& rv) const {
		return Vector3D(_x + rv._x, _y + rv._y, _z + rv._z);
	}

	const Vector3D operator-(const Vector3D& rv) const {
		return Vector3D(_x - rv._x, _y - rv._y, _z - rv._z);
	}

	const Vector3D operator/(double rv) const {
		return Vector3D(_x / rv, _y / rv, _z / rv);
	}

	const Vector3D operator*(double rv) const {
		return Vector3D(_x * rv, _y * rv, _z * rv);
	}

	double operator*(const Vector3D& rv) const {
		return _x * rv._x + _y * rv._y + _z * rv._z;
	}

	//унарный +
	friend const Vector3D& operator+(const Vector3D& i)
	{
		return i;
	}

	//унарный -
	friend const Vector3D operator-(const Vector3D& i)
	{
		return Vector3D(-i._x,-i._y,-i._z);
	}

	double &operator[] (int index);

	double operator[] (int index) const;

private:
	double _x;
	double _y;
	double _z;
};