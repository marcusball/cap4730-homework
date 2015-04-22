#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include "Vertex.h"

struct RenderData{
	GLuint ModelMatrixId;
	glm::mat4 * ModelMatrix;

	GLuint ViewMatrixId;
	glm::mat4 * ViewMatrix;

	GLuint ProjectionMatrixId;
	glm::mat4 * ProjectionMatrix;

	GLuint MVPId; 
};

class RenderableObject
{
public:
	RenderableObject();
	virtual ~RenderableObject();

	/*
	 * Called to render the object to the screen.
	 */
	virtual void Render(RenderData renderData) = 0; // The "= 0" makes this function require an implementation in child classes.

	/*
	 * Called to reset initialized buffers and VAO. 
	 * Essentially clears all data and resets the object to an uninitialized state.
	 */
	virtual void Clear();

protected:
#define INDEX_VB 0
#define POSITION_VB 1
#define COLOR_VB 2
#define NORMAL_VB 3
#define TEXTURE_COORD_VB 4
#define SIZE_VB 5

	virtual void CreateVertexBuffers(const std::vector<Vertex> * const vertices, const std::vector<unsigned int> * const indices);

	bool isInit = false;

	GLuint objectVAO;
	std::array<GLuint, 6> objectBuffers;
};
