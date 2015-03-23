#pragma once
#include "Vectors.h"

class Vertex
{
public:
	Vertex();
	Vertex(Vector4f pos, Vector4f norm);
	~Vertex();

	unsigned int Id;
	Vector4f Position;
	Vector4f Color;
	Vector4f Normal;
};

