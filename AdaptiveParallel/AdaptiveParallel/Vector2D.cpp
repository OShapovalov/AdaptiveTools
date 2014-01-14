#include "Vector2D.h"

double & tplVector2D::operator[] (int index) 
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

double tplVector2D::operator[] (int index) const
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