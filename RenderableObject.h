#pragma once
#include <GL/glew.h>
#include <vector>
#include <array>
#include "Vertex.h"

class RenderableObject
{
public:
	RenderableObject();
	virtual ~RenderableObject();

	/*
	 * Called to render the object to the screen.
	 */
	virtual void Render() = 0; // The "= 0" makes this function require an implementation in child classes.

	/*
	 * Called to reset initialized buffers and VAO. 
	 * Essentially clears all data and resets the object to an uninitialized state.
	 */
	virtual void Clear();

protected:
#define INDEX_VB 0
#define POSITION_VB 1
#define NORMAL_VB 2
#define TEXTURE_COORD_VB

	bool isInit = false;

	GLuint objectVAO;
	std::array<GLuint, 5> objectBuffers;
};
