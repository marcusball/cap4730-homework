#pragma once
#include "Vectors.h"

class Vertex
{
public:
	Vertex();
	Vertex(Vector4f pos);
	Vertex(Vector4f pos, Vector3f norm);
	Vertex(Vector4f pos, Vector3f norm, Vector4f color);
	~Vertex();

	unsigned int Id;
	Vector4f Position;
	Vector4f Color;
	Vector3f Normal;
};

