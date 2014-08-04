#pragma once
#include <math.h>

class PTLVector2D
{
public:

	PTLVector2D():_x(0),_y(0){};
	PTLVector2D(double x, double y):_x(x),_y(y){};
	~PTLVector2D(){};
    PTLVector2D(const PTLVector2D& iVec)
    {
        _x = iVec._x;
        _y = iVec._y;
    }

    PTLVector2D & operator = (PTLVector2D const & iToCopy)
    {
        _x = iToCopy[0];
        _y = iToCopy[1];
        return *this;
    }
    friend bool operator == (const PTLVector2D& rv1, const PTLVector2D& rv2)
    {
        return rv1._x == rv2._x && rv1._y == rv2._y;
    }

	const PTLVector2D operator+(const PTLVector2D& rv) const {
		return PTLVector2D(_x + rv._x, _y + rv._y);
	}

	const PTLVector2D operator-(const PTLVector2D& rv) const {
		return PTLVector2D(_x - rv._x, _y - rv._y);
	}

	const PTLVector2D operator/(double rv) const {
		return PTLVector2D(_x / rv, _y / rv);
	}

	//унарный +
	friend const PTLVector2D& operator+(const PTLVector2D& i)
	{
		return i;
	}

	//унарный -
	friend const PTLVector2D operator-(const PTLVector2D& i)
	{
		return PTLVector2D(-i._x,-i._y);
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