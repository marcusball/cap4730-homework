#include "Point.h"


Point::Point(unsigned int _id):
id(_id){
	XYZW[0] = 0.f;
	XYZW[1] = 0.f;
	XYZW[2] = 0.f;
	XYZW[3] = 1.f;

	RGBA[0] = 0.f;
	RGBA[1] = 0.f;
	RGBA[2] = 0.f;
	RGBA[3] = 1.f;
}


Point::~Point()
{
}

void Point::setRGBA(float r, float g, float b, float a){
	RGBA[0] = r;
	RGBA[1] = g;
	RGBA[2] = b;
	RGBA[3] = a;
}
void Point::setRGBA(const float * rgba){
	RGBA[0] = rgba[0];
	RGBA[1] = rgba[1];
	RGBA[2] = rgba[2];
	RGBA[3] = rgba[3];
}

void Point::invertColor(){
	RGBA[0] = 1.0f - RGBA[0];
	RGBA[1] = 1.0f - RGBA[1];
	RGBA[2] = 1.0f - RGBA[2];
}
