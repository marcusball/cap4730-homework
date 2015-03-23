#pragma comment(lib, "AntTweakBar.lib")
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

#include <GL/glew.h> //Apparently this needs to be before gl.h, and glfw.h. Queen bee of the OpenGL Plastics.
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;

#include <AntTweakBar.h>

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

#include "RenderableObject.h"

// Function prototypes
bool InitializeWindow();
bool InitializeOpenGL();
static void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
static void MouseCallback(GLFWwindow * window, int button, int action, int mods);

void RenderScene();

// Constants
const char* WINDOW_TITLE = "Marcus Ball (38198171)";

const unsigned int RESOLUTION_WIDTH = 1024;
const unsigned int RESOLUTION_HEIGHT = 768;

const float COORD_X_MIN = -4.f;
const float COORD_X_MAX = 4.f;
const float COORD_Y_MIN = -3.f;
const float COORD_Y_MAX = 3.f;

const float CAMERA_EYE_X = 0.f;
const float CAMERA_EYE_Y = 0.f;
const float CAMERA_EYE_Z = -5.f;

// Global Variables
GLFWwindow * Window;
std::string GUIMessage;
std::vector<int> ListenedKeys;
std::vector<int> PressedKeys;

GLuint ProgramID;
GLuint PickingProgramID;

glm::mat4 ProjectionMatrix;
glm::mat4 ViewMatrix;

GLuint MatrixID;
GLuint PickingMatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint LightID;

/***************************************************/
/** Program entry-point and main graphics loop    **/
/***************************************************/
int main(){
	//Initialize the window and create a GUI.
	if (!InitializeWindow()){
		glfwTerminate();
		return 1;
	}

	//Initialize OpenGL
	if (!InitializeOpenGL()){
		glfwTerminate();
		return 1;
	}

	//Perform the main render loop
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	do {
		//// Measure speed
		//double currentTime = glfwGetTime();
		//nbFrames++;
		//if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
		//	// printf and reset
		//	printf("%f ms/frame\n", 1000.0 / double(nbFrames));
		//	nbFrames = 0;
		//	lastTime += 1.0;
		//}

		/*if (animation){
			phi += 0.01;
			if (phi > 360)
				phi -= 360;
		}*/

		// DRAWING POINTS
		RenderScene();

		glfwPollEvents();
	} 
	// Loop and render until ESC is pressed to quit.
	while (glfwGetKey(Window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(Window) == 0);

	return 0;
}

/***************************************************/
/** Initialization and setup functions            **/
/***************************************************/

bool InitializeWindow(){
	//Initialize GLFW
	if (!glfwInit()){
		fprintf(stderr, "Failed to initialize GLFW. Wow, what a loser.\n");
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); //4x anti-aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //What's the most complicated way of expressing "version 3.3"? 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //This. This is.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //We can't use just a normal 'true' value, no. This is OpenGL. We're special. WE'LL MAKE OUR OWN VERSION OF TRUE.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Window = glfwCreateWindow(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (Window == NULL){
		fprintf(stderr, "Failed to open GLFW window. Noob.\n");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(Window); //Initializing the glew thing.
	glewExperimental = true; //Stability is for people with lives and relationships and families. Those aren't supported until OpenGL 5. 
	if (glewInit() != GLEW_OK){
		fprintf(stderr, "Error: init not true\nFailed to initialize GLEW\nIt doesn't like you\n");
		fprintf(stderr, "\nglewHaiku() completed successfully.\n");

		return 1;
	}

	// Initialize the GUI
	/*TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &GUIMessage, NULL);*/

	// Set up inputs
	glfwSetCursorPos(Window, RESOLUTION_WIDTH / 2, RESOLUTION_HEIGHT / 2);
	glfwSetKeyCallback(Window, KeyCallback);
	glfwSetMouseButtonCallback(Window, MouseCallback);

	return true; 
}

bool InitializeOpenGL(){
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	ProjectionMatrix = glm::ortho(COORD_X_MIN, COORD_X_MAX, COORD_Y_MIN, COORD_Y_MAX, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	ViewMatrix = glm::lookAt(
		glm::vec3(CAMERA_EYE_X, CAMERA_EYE_Y, CAMERA_EYE_Z), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);


	ProgramID = LoadShaders("vertex.shader", "frag.shader");
	PickingProgramID = LoadShaders("picking.vertex.shader", "picking.frag.shader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(ProgramID, "MVP");
	ViewMatrixID = glGetUniformLocation(ProgramID, "V");
	ModelMatrixID = glGetUniformLocation(ProgramID, "M");
	PickingMatrixID = glGetUniformLocation(PickingProgramID, "MVP");

	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(ProgramID, "LightPosition_worldspace");

	glfwSetInputMode(Window, GLFW_STICKY_KEYS, GL_TRUE);

	return true;
}

/***************************************************/
/** Render functions                              **/
/***************************************************/

void RenderScene(){
	glUseProgram(ProgramID); //Use the normal shaders
	{
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f); //blue screen of death
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glm::mat4 scaleMatrix, translationMatrix, rotationMatrix;
		glm::mat4 modelMatrix;
		glm::mat4 MVP;

		modelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		MVP = ProjectionMatrix * ViewMatrix * modelMatrix;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		glBlendFunc(GL_NONE, GL_NONE);
		glDisable(GL_BLEND);
	}
	glfwSwapBuffers(Window);
}


/***************************************************/
/** Input callback and event handlers             **/
/***************************************************/
static void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods){
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_A:
			break;
		case GLFW_KEY_D:
			break;
		case GLFW_KEY_W:
			break;
		case GLFW_KEY_S:
			break;
		case GLFW_KEY_SPACE:
			break;
		default:
			break;
		}
	}
}

static void MouseCallback(GLFWwindow * window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		//pickObject();
	}
}