#pragma once

#include "Polygon.h"
#include "Vectors.h"

class LinePolygon : public Polygon
{
public:
	LinePolygon();
	LinePolygon(Vector4f color);
	~LinePolygon();

	void render();
	void setColor(Vector4f color);

protected:
	virtual void pointTransform(std::vector<Point> * const points, std::vector<Point> & newPoints);

	Vector4f lineColor;
};

