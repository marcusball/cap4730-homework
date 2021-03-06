#pragma once
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include <list>
#include <iostream>
#include <time.h>
#include <math.h>
#include <algorithm>
#include <queue>
#include <functional>

#include <GL/glew.h> //Apparently this needs to be before gl.h, and glfw.h. Queen bee of the OpenGL Plastics.
#include <glfw3.h>

#include <glm/glm.hpp>
using namespace glm;

#include "RenderableObject.h"
#include "ControlInterceptor.h"
#include "AssembledObject.h"
#include "MeshObject.h"
#include "Picking.h"
#include "Vectors.h"


class Game{
public:
	//Singleton
	static Game * GetInstance();

	// Constants
	const char* WINDOW_TITLE = "Marcus Ball (38198171)";
	const unsigned int RESOLUTION_WIDTH = 1024;
	const unsigned int RESOLUTION_HEIGHT = 768;

	// Member Variables
	GLFWwindow * Window;

	//Public functions
	int Run();

	PixelData GetPixelSelected();
	Vector2d GetCursorPosition();

private:
	//Singleton
	static Game * singletonInstance;

	// Constants
	const float COORD_X_MIN = -4.f;
	const float COORD_X_MAX = 4.f;
	const float COORD_Y_MIN = -3.f;
	const float COORD_Y_MAX = 3.f;

	const float CAMERA_EYE_X = 10.f;
	const float CAMERA_EYE_Y = 10.f;
	const float CAMERA_EYE_Z = 10.f;

	const int GRID_COUNT = 30;

	// Function prototypes
	bool InitializeWindow();
	bool InitializeOpenGL();
	static void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
	static void MouseCallback(GLFWwindow * window, int button, int action, int mods);
	void RenderScene();
	void DrawPickingBuffer(RenderData renderData);
	void DrawGraphicBuffer(RenderData renderData);
	


	// Member Variables
	std::string GUIMessage;
	std::array<bool, 350> PressedKeys;
	std::array<bool, 8> PressedButtons;
	std::queue<RenderableObject *> RenderQueue;
	std::queue<RenderableObject *> PickingRenderQueue;
	std::vector<ControlInterceptor *> ControlHooks;

	GLuint ProgramID;
	GLuint PickingProgramID;

	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;

	GLuint ProjectionMatrixID;
	
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
	GLuint PickingMatrixID;
	GLuint PickingObjectID;
	GLuint PickingProjectionMatrixID;
	GLuint PickingViewMatrixID;
	GLuint PickingModelMatrixID;
	GLuint LightID1;
	GLuint LightID2;
	

	bool CameraFlipped = false;
	bool debugPicking = false;

	MeshObject * SkinModel;
	AssembledObject * DisplayModel;
	
};