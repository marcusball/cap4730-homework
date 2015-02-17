#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <time.h>
#include <math.h>
#include <algorithm>
#include <queue>

#include <GL/glew.h> //Apparently this needs to be before gl.h, and glfw.h. Queen bee of the OpenGL Plastics.
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "common\shader.hpp"
#include "Point.h"


struct VaoItem{
	GLuint bufferId;
	int itemCount;

	VaoItem(GLuint id, int count) :bufferId(id), itemCount(count){}
};

/** FUNCTION HEADERS **/
const std::vector<Point> getPoints(int numPoints);
GLuint makePointVao(const Point * g_vertex_buffer_data, int numPoints);
void updatePointVao(GLuint vertexBuffer, const Point * g_vertex_buffer_data, int numPoints);
GLuint createIndexBuffer(int numPoints);
void display();
void fakeDraw(GLuint & vao);
float randFloat(float min, float max);
unsigned int getPointIndexById(int searchId);
Point * const getPointById(int searchId);
void placePointOnCircle(int i, int n, Point & p);
void handleModeState();
void registerListenedKey(int key);
bool keyHasBeenPressed(int key);
void dispatchKeyPress(int pressedKey);
void generateAndDrawSecondaryPoints();

/** CONSTANTS **/
const unsigned int POINT_COUNT = 20;
const float POINT_SIZE = 8.0f;
const unsigned int VERTEX_DIMENSIONS = 4; // 3D space
const unsigned int RESOLUTION_WIDTH = 1024;
const unsigned int RESOLUTION_HEIGHT = 768;

const unsigned int MOUSE_PRESSED = 1;
const unsigned int MOUSE_HELD = 2;
const unsigned int MOUSE_HAS_CLICKED = MOUSE_PRESSED | MOUSE_HELD;

const unsigned int MODE_SUBDIV = 64;
const unsigned int MODE_CATMULL = 128;
const unsigned int MODE_BEZIER = 256;

const float RED[4] = { 1.f, 0.f, 0.f, 1.f };
const float YELLOW[4] = { 1.f, 1.f, 0.f, 1.f };
const float GREEN[4] = { 0.f, 1.f, 0.f, 1.f };
const float CYAN[4] = { 0.f, 1.f, 1.f, 1.f };
const float BLUE[4] = { 0.f, 0.f, 1.f, 1.f };
const float PURPLE[4] = { 1.f, 0.f, 1.f, 1.f };
const float WHITE[4] = { 1.f, 1.f, 1.f, 1.f };

/** GLOBAL VARIABLES **/
GLFWwindow * WINDOW;
std::vector<Point> * POINTS;
std::vector<int> LISTENED_KEYS;
std::vector<int> PRESSED_KEYS; 
std::queue<VaoItem> DisplayQueue;

GLuint SecondaryVao = 0;

unsigned int mouseState = 0; //The current mouse click state; 0 -> no click
unsigned int ModeState = 0;
bool ModeLevelChanged = false;


int main(){
	//Initialize GLFW
	if (!glfwInit()){
		fprintf(stderr, "Failed to initialize GLFW. Wow, what a loser.\n");
		return 1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); //4x anti-aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //What's the most complicated way of expressing "version 3.3"? 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //This. This is.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //We can't use just a normal 'true' value, no. This is OpenGL. We're special. WE'LL MAKE OUR OWN VERSION OF TRUE.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

	WINDOW = glfwCreateWindow(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, "Marcus Ball (38198171)", NULL, NULL);
	if (WINDOW == NULL){
		fprintf(stderr, "Failed to open GLFW window. Noob.\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(WINDOW); //Initializing the glew thing.
	glewExperimental = true; //Stability is for people with lives and relationships and families. Those aren't supported until OpenGL 5. 
	if (glewInit() != GLEW_OK){
		fprintf(stderr, "Error: init not true\nFailed to initialize GLEW\nIt doesn't like you\n");
		fprintf(stderr, "\nglewHaiku() completed successfully.\n");

		return 1;
	}

	registerListenedKey(GLFW_KEY_1);
	registerListenedKey(GLFW_KEY_2);
	registerListenedKey(GLFW_KEY_3);

	/*********************************************/
	/** Done with OpenGL set up stuff,          **/
	/** begin actual graphic stuff now.         **/
	/*********************************************/
	std::vector<Point> points = getPoints(POINT_COUNT);
	POINTS = &points;
	const Point * pointVerticies = &(*POINTS)[0];
	GLuint triangleBuffer = makePointVao(pointVerticies, POINT_COUNT);
	GLuint indexBuffer = createIndexBuffer(POINT_COUNT);

	GLuint programID = LoadShaders("vertex.shader", "frag.shader");
	GLuint pickingProgramID = LoadShaders("picking.vertex.shader", "picking.frag.shader");

	glfwSetInputMode(WINDOW, GLFW_STICKY_KEYS, GL_TRUE);
	

	Point * selectedPoint = NULL; //Pointer to the selected Point object; populated when user clicks on a point. 

	//Just some variables that will be used to calculate the conversion between screen coordinates and vertex coordinates.
	float m_x = 2.0f / RESOLUTION_WIDTH;
	float m_y = -2.0f / RESOLUTION_HEIGHT;
	float x_0, x_1;
	float y_0, y_1;
	do{
		
		// update vertex buffer each frame so that shaders can use updated vertex data (HINT: useful when dragging, changing color)
		glBindBuffer(GL_ARRAY_BUFFER, triangleBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, POINT_COUNT * VERTEX_DIMENSIONS * sizeof(GLfloat), pointVerticies);	// update buffer data

		// PICKING IS DONE HERE
		if (glfwGetMouseButton(WINDOW, GLFW_MOUSE_BUTTON_LEFT)){
			mouseState |= MOUSE_PRESSED; 

			glUseProgram(pickingProgramID);
			{
				fakeDraw(triangleBuffer);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				// Read the pixel at the center of the screen.
				// You can also use glfwGetMousePos().
				// Ultra-mega-over slow too, even for 1 pixel, 
				// because the framebuffer is on the GPU.
				double xpos, ypos;
				glfwGetCursorPos(WINDOW, &xpos, &ypos);
				unsigned char data[4];
				glReadPixels(xpos, RESOLUTION_HEIGHT - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

				// Convert the color back to an integer ID
				int pickedID = int(data[0]);

				
				if (pickedID != 255){ //Not the background
					if (mouseState & MOUSE_HELD){ //If the mouse has already been held down for some amount of time...
						if (selectedPoint != NULL){ //Make sure we actually got a selected point, and it's not a null pointer
							float x_n = m_x * (xpos - x_0) + x_1; //Calculate the new point position; map (0 - screen_width) -> (-1 - 1)
							float y_n = m_y * (ypos - y_0) + y_1; //Calculate new Y point position; map (0 - screen_height) -> (-1 - 1)

							//Update the position
							selectedPoint->XYZW[0] = x_n;
							selectedPoint->XYZW[1] = y_n;

							//std::cout << "updating " << selectedPoint->id << " to (" << x_n << ", " << y_n << ") " << std::endl;
						}
					}
					else if (mouseState & MOUSE_PRESSED){ //If the mouse just now got clicked for the first time (eg. click begins)
						selectedPoint = getPointById(pickedID); //get a pointer to the selected Point object
						if (selectedPoint != NULL){ //Make sure something didn't go wrong
							selectedPoint->setRGBA(RED);
							//selectedPoint->invertColor();

							//Update the reference points so our conversion math works in the next states (see above).
							x_0 = xpos;
							x_1 = selectedPoint->XYZW[0];

							y_0 = ypos;
							y_1 = selectedPoint->XYZW[1];
						}
						else{
							//std::cout << "ERROR: Unable to locate selected Point (" << pickedID << ")!" << std::endl;
						}

						//Update the mouse state
						mouseState = MOUSE_HELD;
					}
				}
				else{
					//This is for the case where you move your mouse so fast that the cursor actually stops pointing at the box,
					//but you were still holding the mouse button. This enables the box to catch up to the cursor.
					if (mouseState & MOUSE_HELD){ 
						if (selectedPoint != NULL){
							float x_n = m_x * (xpos - x_0) + x_1;
							float y_n = m_y * (ypos - y_0) + y_1;

							selectedPoint->XYZW[0] = x_n;
							selectedPoint->XYZW[1] = y_n;
						}
					}
				}
			}
		}
		else{ //The mouse is not being clicked
			if (mouseState & MOUSE_HAS_CLICKED){ //but the mouse WAS clicked before (so it's been released).
				if (selectedPoint != NULL){
					//selectedPoint->setRGBA(1.f, 0.f, 0.f, 1.f); //Return the point to its original color.
					//selectedPoint->invertColor();
					selectedPoint->setRGBA(WHITE);
				}

				selectedPoint = NULL; //clear it, just to be safe
				mouseState = 0; //Return the mouse state to 0 (no click events present)
			}
		}

		updatePointVao(triangleBuffer, pointVerticies, POINT_COUNT); // Refresh the vertices

		glUseProgram(programID); //Use the normal shaders
		//display(triangleBuffer); //Draw the boxes
		DisplayQueue.push(VaoItem(triangleBuffer, POINT_COUNT));

		generateAndDrawSecondaryPoints();

		display();

		glfwPollEvents();

		handleModeState(); //Check for input and change the mode state appropriately
	} while (glfwGetKey(WINDOW, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(WINDOW) == 0);
}

/* 
 * Draw the boxes to the screen!
 */
void display(){
	static const size_t pointIdOffset = 0;
	static const size_t pointVertexOffset = pointIdOffset + sizeof((*POINTS)[0].id);
	static const size_t pointColorOffset = pointVertexOffset + sizeof((*POINTS)[0].XYZW);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f); //blue screen of death
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(POINT_SIZE);
	glEnable(GL_POINT_SMOOTH);

	while (!DisplayQueue.empty()){
		VaoItem vao = DisplayQueue.front();

		glBindBuffer(GL_ARRAY_BUFFER, vao.bufferId);
		glVertexAttribPointer(0, VERTEX_DIMENSIONS, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)pointVertexOffset); //Verticies 
		glVertexAttribPointer(1, VERTEX_DIMENSIONS, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)pointColorOffset); //Colors

		glDrawArrays(GL_POINTS, 0, vao.itemCount * sizeof(Point));

		DisplayQueue.pop();

	}

	glDisable(GL_POINT_SMOOTH);
	glBlendFunc(GL_NONE, GL_NONE);
	glDisable(GL_BLEND);
	glDisableVertexAttribArray(0);

	//glFlush();
	//glFinish();

	glfwSwapBuffers(WINDOW);
}

/*
 * Draw the boxes to the buffer, but don't swap them into the window buffer.
 * Used to keep track of where on the screen the user is clicking.
 */
void fakeDraw(GLuint & vao){
	static const size_t pointIdOffset = 0;
	static const size_t pointVertexOffset = pointIdOffset + sizeof((*POINTS)[0].id);
	static const size_t pointColorOffset = pointVertexOffset + sizeof((*POINTS)[0].XYZW);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //blue screen of death
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(POINT_SIZE);

	glBindBuffer(GL_ARRAY_BUFFER, vao);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(Point), (GLvoid*)pointIdOffset); //Id 
	glVertexAttribPointer(1, VERTEX_DIMENSIONS, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)pointVertexOffset); //Verticies

	glDrawArrays(GL_POINTS, 0, POINT_COUNT * sizeof(Point));

	glDisable(GL_POINT_SMOOTH);
	glBlendFunc(GL_NONE, GL_NONE);
	glDisable(GL_BLEND);
	glDisableVertexAttribArray(0);

	glFlush();
	glFinish();

	//glfwSwapBuffers(WINDOW);
}

const std::vector<Point> getPoints(int numPoints){
	srand(time(NULL)); //for rand()
	std::vector<Point> points(0);

	for (int i = 0; i < numPoints; i += 1){
		Point p(i);
		placePointOnCircle(i, numPoints, p);
		p.setRGBA(WHITE);

		points.push_back(p);
	}

	return points;
}

GLuint makePointVao(const Point * g_vertex_buffer_data, int numPoints){
	/* Supposed to do this first, because they said so. */
	GLuint VertexArrayID; 
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(Point), g_vertex_buffer_data, GL_STATIC_DRAW);

	return vertexBuffer;
}

void updatePointVao(GLuint vertexBuffer, const Point * g_vertex_buffer_data, int numPoints){
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numPoints * sizeof(Point), g_vertex_buffer_data);
}

GLuint createIndexBuffer(int numPoints){
	std::vector<unsigned short> indicies(numPoints);
	for (int i = 0; i < numPoints; i += 1){
		indicies[i] = i;
	}

	GLuint indexBufferId;
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numPoints * sizeof(unsigned short), &indicies[0], GL_STATIC_DRAW);

	return indexBufferId;
}

/*
 * Get a random float value between min and max.
 */
float randFloat(float min, float max){
	return min + (rand() / (RAND_MAX / (max - min)));
}

/*
 * Searches the POINTS vector for a Point with the given ID.
 * return: the index of the point within the POINTS vector.
 */
unsigned int getPointIndexById(int searchId){
	for (int i = 0; i < POINTS->size(); i += 1){
		if ((*POINTS)[i].id == searchId){
			return i;
		}
	}
	return -1;
}

/*
 * If all goes well, this will return a pointer to the Point object
 * return: pointer to the Point object, or NULL if not found.
 */
Point * const getPointById(int searchId){
	unsigned int index = getPointIndexById(searchId);
	if (index != -1){
		return &(*POINTS)[index];
	}
	return NULL;
}

/*
 * Given a reference to a Point (p), and the index (i) of that point out of the total number (n) of points,
 * set the coordinates of the point such that it, combined with all points, make up a circle. 
 * Note, i should be zero indexed, such that i = [0,n). 
 */
void placePointOnCircle(int i, int n, Point & p){
	float scaleFactorX = (float)RESOLUTION_HEIGHT / (float)RESOLUTION_WIDTH; //factor for wide windows 
	if (RESOLUTION_HEIGHT > RESOLUTION_WIDTH){ scaleFactorX = 1.0 / scaleFactorX; } //in case the window is taller than it is wide
	//This factor is to scale the points, since the bounds of this window are -1 and 1 on box axis', regardless of the window dimensions

	float radius = 0.75f;

	float rad = ((float)i / (float)n) * 2.0 * M_PI; //Out of a full circle, where should this point be

	p.XYZW[0] = std::cos(rad) * radius * scaleFactorX;
	p.XYZW[1] = std::sin(rad) * radius;
	p.XYZW[2] = 0.f;
	p.XYZW[3] = 1.f;
}

//Add a keyboard key to the list of keys to be listened for presses.
void registerListenedKey(int key){
	if (std::find(LISTENED_KEYS.begin(), LISTENED_KEYS.end(), key) == LISTENED_KEYS.end()){ //If key hasn't been added already
		LISTENED_KEYS.push_back(key);
	}
}

bool keyHasBeenPressed(int key){
	if (std::find(PRESSED_KEYS.begin(), PRESSED_KEYS.end(), key) != PRESSED_KEYS.end()){
		return true;
	}
	return false;
}

void dispatchKeyPress(int pressedKey){
	switch (pressedKey){
		case(GLFW_KEY_1):
			if (ModeState & MODE_SUBDIV){
				int value = ((ModeState & 0x0f) + 1) % 16; //We'll allow a max of 16 different subdiv states
				ModeState &= ~0x0f; //Clear out the last 4 bits, reset to zero. 
				ModeState |= value; //OR in the new value
				ModeLevelChanged = true;

				std::cout << "Subdiv level is now " << value << std::endl;
			}
			else{
				ModeState = MODE_SUBDIV;
				std::cout << "Setting mode to SUBDIV" << std::endl;
			}
			break;

		case(GLFW_KEY_2):
			if (ModeState & MODE_CATMULL){
				int value = ((ModeState & 0x0f) + 1) % 16; //We'll allow a max of 16 different subdiv states
				ModeState &= ~0x0f; //Clear out the last 4 bits, reset to zero. 
				ModeState |= value; //OR in the new value
			}
			else{
				ModeState = MODE_CATMULL;
				std::cout << "Setting mode to CATMULL" << std::endl;
			}
			break;
		case(GLFW_KEY_3):
			if (ModeState & MODE_BEZIER){
				int value = ((ModeState & 0x0f) + 1) % 16; //We'll allow a max of 16 different subdiv states
				ModeState &= ~0x0f; //Clear out the last 4 bits, reset to zero. 
				ModeState |= value; //OR in the new value
			}
			else{
				ModeState = MODE_BEZIER;
				std::cout << "Setting mode to BEZIER" << std::endl;
			}
			break;
	}
}

/*
 * Loops through all of the keys registered in LISTENED_KEYS to check for key presses.
 * Will call dispatchKeyPress with the key value when a key is pressed and released. 
 */
void handleModeState(){
	for (int i = 0; i < LISTENED_KEYS.size(); i += 1){
		if (glfwGetKey(WINDOW, LISTENED_KEYS[i]) == GLFW_PRESS){
			if (!keyHasBeenPressed(LISTENED_KEYS[i])){
				PRESSED_KEYS.push_back(LISTENED_KEYS[i]);
			}
		}
		if (glfwGetKey(WINDOW, LISTENED_KEYS[i]) == GLFW_RELEASE){
			if (keyHasBeenPressed(LISTENED_KEYS[i])){
				//Remove the key from the list of pressed keys
				PRESSED_KEYS.erase(std::remove(PRESSED_KEYS.begin(), PRESSED_KEYS.end(), LISTENED_KEYS[i]), PRESSED_KEYS.end());
				
				dispatchKeyPress(LISTENED_KEYS[i]); //Do something now that the key has been pressed (and released)
			}
		}
	}
}

void generateAndDrawSecondaryPoints(){
	if (ModeState & MODE_SUBDIV){ //We're doing subdivision now
		unsigned int subdivLevel = ModeState & 0x0f;
		if (subdivLevel == 0){
			return; //If level is zero, we're not even going to draw any points
		}

		int secondaryPointCount = POINT_COUNT * std::pow(2, subdivLevel);
		int pointSetCount = subdivLevel + 1;
		std::vector<std::vector<Point>> secondaryPoints;
		secondaryPoints.push_back(*POINTS);

		std::vector<Point> tempVector;
		std::vector<std::vector<Point>> & P = secondaryPoints; //Lazy alias
		for (int k = 1; k < pointSetCount; k += 1){
			int kCount = POINT_COUNT * std::pow(2, k);
			std::vector<Point> temp(kCount);
			P.push_back(std::vector<Point>(kCount));

			for (int i = 0; i < kCount / 2; i += 1){
				int im1 = (i - 1 >= 0) ? (i - 1) : (kCount/2 - 1);
				int ip1 = (i + 1 < kCount/2) ? (i + 1) : 0;

				P[k][2 * i] = (P[k - 1][im1] * 4.0f + P[k - 1][i] * 4.f) / 8.0f;
				P[k][2 * i].setRGBA(CYAN);

				P[k][2 * i + 1] = (P[k - 1][im1] + (P[k - 1][i] * 6) + P[k - 1][ip1]) / 8.0f;
				P[k][2 * i + 1].setRGBA(CYAN);
			}
		}

		Point * pointVerticies = &(secondaryPoints[subdivLevel][0]);
		if (SecondaryVao == 0){
			SecondaryVao = makePointVao(pointVerticies,secondaryPointCount);
		}
		else{
			if (ModeLevelChanged){
				glDeleteBuffers(1, &SecondaryVao);
				SecondaryVao = makePointVao(pointVerticies, secondaryPointCount);
				ModeLevelChanged = false;
			}
			else{
				updatePointVao(SecondaryVao, pointVerticies, secondaryPointCount);
			}
		}
		
		DisplayQueue.push(VaoItem(SecondaryVao, secondaryPointCount));
	}
}