#pragma once
#include <math.h>

class Vector2D
{
public:

	Vector2D():_x(0),_y(0){};
	Vector2D(double x, double y):_x(x),_y(y){};
	~Vector2D(){};
    Vector2D(const Vector2D& iVec)
    {
        _x = iVec._x;
        _y = iVec._y;
    }

    Vector2D & operator = (Vector2D const & num)
    {
        return *this;
    }
    friend bool operator == (const Vector2D& rv1, const Vector2D& rv2)
    {
        return rv1._x == rv2._x && rv1._y == rv2._y;
    }

	const Vector2D operator+(const Vector2D& rv) const {
		return Vector2D(_x + rv._x, _y + rv._y);
	}

	const Vector2D operator-(const Vector2D& rv) const {
		return Vector2D(_x - rv._x, _y - rv._y);
	}

	const Vector2D operator/(double rv) const {
		return Vector2D(_x / rv, _y / rv);
	}

	//унарный +
	friend const Vector2D& operator+(const Vector2D& i)
	{
		return i;
	}

	//унарный -
	friend const Vector2D operator-(const Vector2D& i)
	{
		return Vector2D(-i._x,-i._y);
	}

	double &operator[] (int index);

	double operator[] (int index) const;

	double Magnitude() const
	{
		return sqrt(_x*_x + _y*_y);
	}

private:
	double _x;
	double _y;
};