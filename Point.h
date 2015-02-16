#pragma once
class Point
{
public:
	Point(unsigned int _id = 0);
	~Point();

	const unsigned int id;

	float XYZW[4];
	float RGBA[4];

	void setRGBA(float r, float g, float b, float a);
	void setRGBA(const float * rgba);
	void invertColor();
};

