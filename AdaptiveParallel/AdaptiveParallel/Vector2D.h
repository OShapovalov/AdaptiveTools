#pragma once
#include <math.h>

class Vector2D
{
public:

	Vector2D():_x(0),_y(0){};
	Vector2D(double x, double y):_x(x),_y(y){};
	~Vector2D(){};

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