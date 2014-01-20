#pragma once
#include <math.h>

class tplVector2D
{
public:

	tplVector2D():_x(0),_y(0){};
	tplVector2D(double x, double y):_x(x),_y(y){};
	~tplVector2D(){};
    tplVector2D(const tplVector2D& iVec)
    {
        _x = iVec._x;
        _y = iVec._y;
    }

    tplVector2D & operator = (tplVector2D const & iToCopy)
    {
        _x = iToCopy[0];
        _y = iToCopy[1];
        return *this;
    }
    friend bool operator == (const tplVector2D& rv1, const tplVector2D& rv2)
    {
        return rv1._x == rv2._x && rv1._y == rv2._y;
    }

	const tplVector2D operator+(const tplVector2D& rv) const {
		return tplVector2D(_x + rv._x, _y + rv._y);
	}

	const tplVector2D operator-(const tplVector2D& rv) const {
		return tplVector2D(_x - rv._x, _y - rv._y);
	}

	const tplVector2D operator/(double rv) const {
		return tplVector2D(_x / rv, _y / rv);
	}

	//унарный +
	friend const tplVector2D& operator+(const tplVector2D& i)
	{
		return i;
	}

	//унарный -
	friend const tplVector2D operator-(const tplVector2D& i)
	{
		return tplVector2D(-i._x,-i._y);
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