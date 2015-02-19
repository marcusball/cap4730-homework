#pragma once
#include <GL/glew.h>
#include <vector>
#include <array>
#include "Point.h"

class Polygon
{
public:
	Polygon();
	virtual ~Polygon();

	void init(std::vector<Point> * const points);
	void render();

private:
	void createCircle(std::vector<Point> * const points);
	void handlePoint(const Point point, std::vector<unsigned int> & ids, std::vector<Vector4f> & positions, std::vector<Vector4f> & colors, std::vector<float> & pointSizes);

	virtual void pointTransform(std::vector<Point> * const points, std::vector<Point> & newPoints);
	//void clear();

#define INDEX_VB 0
#define ID_BUFFER 1
#define POSITION_VB 2
#define COLOR_VB 3
#define POINT_SIZE_VB 4


	unsigned int vertexCount;
	
	GLuint polygonVAO;
	std::array<GLuint, 5> polygonBuffers;
};

