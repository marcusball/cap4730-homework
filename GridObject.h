#pragma once
#include <vector>

#include "RenderableObject.h"
#include "Vectors.h"
#include "Vertex.h"

class GridObject : public RenderableObject
{
public:
	/*
	 * Create a renderable single-plane grid. The grid will be a square, within the bounds of topLeft and bottomRight.
	 * The grid will be within the plane in which normalAxis is the normal vector.
	 * The grid will be broken up into rows and columns, both of which will contain blockCount divisions.
	 */
	GridObject();
	~GridObject();

	void Init(float sideLength, int blockCount = 10);

	virtual void Render();

private:
	/*
	 * Generate all of the necessary verticies and data to display the grid. 
	 */
	void GenerateVertices(float sideLength, int blockCount, std::vector<unsigned int> & outIndices, std::vector<Vertex> & outVertices);

	int lineCount = 0;
};

