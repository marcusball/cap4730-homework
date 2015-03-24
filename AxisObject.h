#pragma once
#include <vector>

#include "RenderableObject.h"
#include "Vectors.h"
#include "Vertex.h"

class AxisObject : public RenderableObject
{
public:
	AxisObject();
	~AxisObject();

	void Init(float lineLength = 5.f);

	virtual void Render();

private:
	/*
	* Generate all of the necessary verticies and data to display the grid.
	*/
	void GenerateVertices(float lineLength, std::vector<unsigned int> & outIndices, std::vector<Vertex> & outVertices);
};
