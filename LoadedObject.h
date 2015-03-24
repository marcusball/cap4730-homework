#pragma once
#include <vector>

#include "RenderableObject.h"
#include "Vectors.h"
#include "Vertex.h"

class LoadedObject : public RenderableObject
{
public:
	LoadedObject();
	~LoadedObject();

	void LoadFromFile(std::string filename);

	void SetColor(Vector4f color);

	virtual void Render();

private:

	static bool LoadObject(std::string filepath, std::vector<Vertex> & vertices, std::vector<unsigned int> & indices);

	int vertexCount;

	bool overrideColorSet = false;
	Vector4f overrideColor;
};

