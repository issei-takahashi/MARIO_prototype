#pragma once
#include "utils.hpp"

class Point3D_mm
{
public:
	Point3D_mm()
		:x(0),y(0),z(0)
	{}
	Point3D_mm(int x, int y, int z)
		:x(x),y(y),z(z)
	{}
	// =演算子オーバーロード
	Point3D_mm& operator=(const Point3D_mm& obj)
	{
		this->x = obj.x;
		this->y = obj.y;
		this->z = obj.z;
		return *this;
	}
	// -演算子オーバーロード
	Point3D_mm operator-(const Point3D_mm& obj) const
	{
		Point3D_mm tmp;
		tmp.x = x - obj.x;
		tmp.y = y - obj.y;
		tmp.z = z - obj.z;
		return tmp;
	}
	// +=演算子オーバーロード
	Point3D_mm& operator+=(const Point3D_mm& obj)
	{
		x += obj.x;
		y += obj.y;
		z += obj.z;
		return *this;
	}

	double x,y,z;
};