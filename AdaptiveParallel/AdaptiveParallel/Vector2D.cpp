#include "Vector2D.h"

double & Vector2D::operator[] (int index) 
{
	if (index == 0) 
	{
		return _x;
	}
	else
	{
		return _y;
	}
}

double Vector2D::operator[] (int index) const
{
	if (index == 0) 
	{
		return _x;
	}
	else
	{
		return _y;
	}
}