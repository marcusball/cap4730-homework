#pragma once
#include "Vectors.h"

class Point
{
public:
	Point(unsigned int _id = 0);
	Point(float x, float y, float z, float w = 1);
	~Point();

	const unsigned int id;
	Vector4f XYZW;
	Vector4f RGBA;
	float pointSize = 10.0f;

	void setRGBA(float r, float g, float b, float a);
	void setRGBA(const float * rgba);
	void setRGBA(Vector4f color);
	void invertColor();

	Point operator -(const Point& a)const {
		return Point(this->XYZW[0] - a.XYZW[0], this->XYZW[1] - a.XYZW[1], this->XYZW[2] - a.XYZW[2]);
	}
	Point operator +(const Point& a)const {
		return Point(this->XYZW[0] + a.XYZW[0], this->XYZW[1] + a.XYZW[1], this->XYZW[2] + a.XYZW[2]);
	}
	Point operator *(const float& a) const {
		return Point(this->XYZW[0] * a, this->XYZW[1] * a, this->XYZW[2] * a);
	}
	Point operator /(const float& a)const {
		return Point(this->XYZW[0] / a, this->XYZW[1] / a, this->XYZW[2] / a);
	}
	Point operator =(const Point& a){
		if (&a == this){
			return *this;
		}

		this->XYZW[0] = a.XYZW[0];
		this->XYZW[1] = a.XYZW[1];
		this->XYZW[2] = a.XYZW[2];
		this->XYZW[3] = a.XYZW[3];

		return *this;
	}
};

