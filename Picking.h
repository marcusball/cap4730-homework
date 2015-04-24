#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include "Vertex.h"

struct PixelData{
	float ObjectId;
	float PointId;
	float PrimitiveId;

	PixelData() : ObjectId(0.f), PointId(0.f), PrimitiveId(0.f){}
};

class Picking{
public:
	static bool Initialize();

	/*
	 * Modifies the OpenGL frame buffer object. When enabled, all draws
	 * will be used to generate identification for picking. Once the id
	 * scene has been drawn, call Disable() to complete.
	 */
	static void Enable();
	static void Disable();

	static PixelData ReadPixelAt(Vector2ui coord);

	static void UpdateObjectId(GLuint objectIdLocation, unsigned int objectId);
private:
	
	static GLuint FBO;
	static GLuint idTexture;
	static GLuint depthTexture;

	/*
	 * Toggles whether VBO is bound or not.
	 */
	static void BindToFBO(bool isBound);
};