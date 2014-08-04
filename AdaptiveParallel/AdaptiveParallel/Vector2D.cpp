#include "Vector2D.h"

double & PTLVector2D::operator[] (int index) 
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

double PTLVector2D::operator[] (int index) const
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