#pragma once
#include <vector>

#include "RenderableObject.h"
#include "Vectors.h"
#include "Vertex.h"

class MeshObject : public RenderableObject
{
public:
	/*
	* Create a renderable single-plane grid. The grid will be a square, within the bounds of topLeft and bottomRight.
	* The grid will be within the plane in which normalAxis is the normal vector.
	* The grid will be broken up into rows and columns, both of which will contain blockCount divisions.
	*/
	MeshObject();
	~MeshObject();

	void Init(float sideLength, int blockCount = 12);

	virtual void Render(RenderData renderData);

private:
	/*
	* Generate all of the necessary verticies and data to display the grid.
	*/
	void GenerateVertices(float sideLength, int pointsPerSide, std::vector<unsigned int> & outIndices, std::vector<Vertex> & outVertices, int & pointIndexCount, int & lineIndexCount);

	static int GetNeighborIndex(int currentIndex, int horizontalLength, int verticalLength, int direction);

	int lineCount = 0;

	int pointVertexCount;
	int lineVertexCount;
};

