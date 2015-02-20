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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "common\shader.hpp"
#include "Point.h"
#include "Polygon.h"
#include "LinePolygon.h"

enum PolygonTypes{
	Points,
	Lines
};

struct PolygonQueueItem{
	PolygonQueueItem(Polygon & p, PolygonTypes t):polygon(p), type(t){}
	Polygon & polygon;
	PolygonTypes type;
};

/** FUNCTION HEADERS **/
const std::vector<Point> getPoints(int numPoints);
void drawPolygon(Polygon & polygon, bool swapToWindow);
void drawPolygons();
float randFloat(float min, float max);
unsigned int getPointIndexById(int searchId);
Point * const getPointById(int searchId);
void placePointOnCircle(int i, int n, Point & p);
bool handleModeState();
void registerListenedKey(int key);
bool keyHasBeenPressed(int key);
void dispatchKeyPress(int pressedKey);
void generateSecondaryPoints();
float getPointDistance(Point a, Point b);
Vector4f normalizeVector(Vector4f input);
Point deCasteljau(const std::vector<Point> * P, float t);

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

const unsigned int DRAW_MODE_POINTS = 0;
const unsigned int DRAW_MODE_LINES = 1;

const Vector4f RED = { 1.f, 0.f, 0.f, 1.f };
const Vector4f YELLOW = { 1.f, 1.f, 0.f, 1.f };
const Vector4f GREEN = { 0.f, 1.f, 0.f, 1.f };
const Vector4f CYAN = { 0.f, 1.f, 1.f, 1.f };
const Vector4f BLUE = { 0.f, 0.f, 1.f, 1.f };
const Vector4f PURPLE = { 1.f, 0.f, 1.f, 1.f };
const Vector4f WHITE = { 1.f, 1.f, 1.f, 1.f };

/** GLOBAL VARIABLES **/
GLFWwindow * WINDOW;
std::vector<Point> * POINTS;
std::vector<int> LISTENED_KEYS;
std::vector<int> PRESSED_KEYS; 
std::queue<PolygonQueueItem> DisplayQueue;

Polygon primaryPolygon;
LinePolygon primaryLinePolygon;
Polygon secondaryPolygon;
LinePolygon secondaryLinePolygon;

unsigned int mouseState = 0; //The current mouse click state; 0 -> no click
unsigned int ModeState = 0;
bool ModeLevelChanged = false;
bool DEBUG_SELECTION_DRAW = false;
bool HideSelectionPoints = false;


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
	registerListenedKey(GLFW_KEY_H);
	registerListenedKey(GLFW_KEY_D);

	/*********************************************/
	/** Done with OpenGL set up stuff,          **/
	/** begin actual graphic stuff now.         **/
	/*********************************************/
	std::vector<Point> points = getPoints(POINT_COUNT);
	POINTS = &points;

	primaryPolygon.init(POINTS);
	primaryLinePolygon.setColor(WHITE);
	primaryLinePolygon.init(POINTS);

	GLuint programID = LoadShaders("vertex.shader", "frag.shader");
	GLuint pickingProgramID = LoadShaders("picking.vertex.shader", "picking.frag.shader");

	glfwSetInputMode(WINDOW, GLFW_STICKY_KEYS, GL_TRUE);
	

	Point * selectedPoint = NULL; //Pointer to the selected Point object; populated when user clicks on a point. 
	bool requiresRefresh = true;
	//Just some variables that will be used to calculate the conversion between screen coordinates and vertex coordinates.
	float m_x = 2.0f / RESOLUTION_WIDTH;
	float m_y = -2.0f / RESOLUTION_HEIGHT;
	float x_0, x_1;
	float y_0, y_1;
	do{
		if (requiresRefresh){
			primaryPolygon.update(POINTS);
			primaryLinePolygon.update(POINTS);
			generateSecondaryPoints();

			requiresRefresh = false;
		}

		// PICKING IS DONE HERE
		if (glfwGetMouseButton(WINDOW, GLFW_MOUSE_BUTTON_LEFT)){
			mouseState |= MOUSE_PRESSED; 

			glUseProgram(pickingProgramID);
			{
				//HiddenDisplayQueue.push(VaoItem(pointBuffer, pointBufferArray, POINT_COUNT, DRAW_MODE_POINTS));

				drawPolygon(primaryPolygon, false);

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
							selectedPoint->pointSize *= 2.f;
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
					requiresRefresh = true;
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
					requiresRefresh = true;
				}
			}
		}
		else{ //The mouse is not being clicked
			if (mouseState & MOUSE_HAS_CLICKED){ //but the mouse WAS clicked before (so it's been released).
				if (selectedPoint != NULL){
					//selectedPoint->setRGBA(1.f, 0.f, 0.f, 1.f); //Return the point to its original color.
					//selectedPoint->invertColor();
					selectedPoint->setRGBA(WHITE);
					selectedPoint->pointSize /= 2.f;
				}

				selectedPoint = NULL; //clear it, just to be safe
				mouseState = 0; //Return the mouse state to 0 (no click events present)
				requiresRefresh = true;
			}
		}

		//updatePointVao(pointBuffer, pointVerticies, POINT_COUNT); // Refresh the vertices

		glUseProgram(programID); //Use the normal shaders

		DisplayQueue.push(PolygonQueueItem(secondaryLinePolygon, Lines));
		DisplayQueue.push(PolygonQueueItem(secondaryPolygon, Points));

		if (!HideSelectionPoints){
			DisplayQueue.push(PolygonQueueItem(primaryLinePolygon, Lines));
			DisplayQueue.push(PolygonQueueItem(primaryPolygon, Points));
		}


		drawPolygons();

		glfwPollEvents();
		requiresRefresh |= handleModeState(); //Check for input and change the mode state appropriately
	} while (glfwGetKey(WINDOW, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(WINDOW) == 0);
}

void drawPolygon(Polygon & polygon, bool swapToWindow){
	if (swapToWindow){
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f); //blue screen of death
	}
	else{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	polygon.render();

	glBlendFunc(GL_NONE, GL_NONE);
	glDisable(GL_BLEND);


	if (swapToWindow ^ DEBUG_SELECTION_DRAW){ //If we're not debugging,
		glfwSwapBuffers(WINDOW); //just swap the buffer to the window and we're good
	}
	else{ //If we are debugging, we aren't going to show this on the window
		glFlush();
		glFinish();
	}
}

void drawPolygons(){
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f); //blue screen of death
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!DisplayQueue.empty()){
		PolygonQueueItem top = DisplayQueue.front();

		switch (top.type){
		case (Points):
			top.polygon.render();
			break;
		case(Lines) :
			LinePolygon & polygonCast = static_cast<LinePolygon &>(top.polygon);
			polygonCast.render();
		}
		

		DisplayQueue.pop();
	}

	glBlendFunc(GL_NONE, GL_NONE);
	glDisable(GL_BLEND);


	if (!DEBUG_SELECTION_DRAW){ //If we're not debugging,
		glfwSwapBuffers(WINDOW); //just swap the buffer to the window and we're good
	}
	else{ //If we are debugging, we aren't going to show this on the window
		glFlush();
		glFinish();
	}
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
				int value = ((ModeState & 0x0f) + 0x01) % 11; //We'll allow a max of 16 different subdiv states
				ModeState &= ~0x0f; //Clear out the last 4 bits, reset to zero. 
				ModeState |= value; //OR in the new value
				ModeLevelChanged = true;

				std::cout << "Subdiv level is now " << value << std::endl;
			}
			else{
				secondaryPolygon.clear();
				secondaryLinePolygon.clear();

				ModeState = MODE_SUBDIV;
				std::cout << "Setting mode to SUBDIV" << std::endl;
			}
			break;

		case(GLFW_KEY_2):
			secondaryPolygon.clear();
			secondaryLinePolygon.clear();

			if(ModeState & MODE_CATMULL){
				int value = ((ModeState & 0x01) + 1) % 2; //We'll allow a max of 2 different subdiv states
				ModeState &= ~0x01; //Clear out the last 4 bits, reset to zero. 
				ModeState |= value; //OR in the new value
				ModeLevelChanged = true;

				std::cout << "Catmull mode is now " << value << std::endl;
			}
			else{
				ModeState = MODE_CATMULL;
				std::cout << "Setting mode to CATMULL" << std::endl;
			}
			break;
		case(GLFW_KEY_3):
			secondaryPolygon.clear();
			secondaryLinePolygon.clear();

			ModeState = MODE_BEZIER;
			std::cout << "Setting mode to BEZIER" << std::endl;
			break;
		case(GLFW_KEY_D) :
			DEBUG_SELECTION_DRAW = !DEBUG_SELECTION_DRAW;
			break;
		case(GLFW_KEY_H) :
			HideSelectionPoints = !HideSelectionPoints;
			if (HideSelectionPoints){
				std::cout << "Hiding selection points" << std::endl;
			}
			else{
				std::cout << "Showing selection points" << std::endl;
			}
			break;
	}
}

/*
 * Loops through all of the keys registered in LISTENED_KEYS to check for key presses.
 * Will call dispatchKeyPress with the key value when a key is pressed and released. 
 * Returns true if any input has been passed
 */
bool handleModeState(){
	bool keyPressed = false;
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
				keyPressed = true;
			}
		}
	}
	return keyPressed;
}

void generateSecondaryPoints(){
	if (ModeState & MODE_SUBDIV){ //We're doing subdivision now
		unsigned int subdivLevel = ModeState & 0x0f;
		subdivLevel += 1;

		long secondaryPointCount = POINT_COUNT * std::pow(2, subdivLevel);
		int pointSetCount = subdivLevel + 1;
		std::vector<Point> * P_km1 = POINTS; //Initialize P_k-1 as POINTS, because this is P_k where k=0.
		std::vector<Point> * P_k;

		for (int k = 1; k < pointSetCount; k += 1){
			long kCount = POINT_COUNT * std::pow(2, k);
			P_k = new std::vector<Point>(kCount);

			for (int i = 0; i < kCount / 2; i += 1){
				int im1 = (i - 1 >= 0) ? (i - 1) : (kCount/2 - 1);
				int ip1 = (i + 1 < kCount/2) ? (i + 1) : 0;

				(*P_k)[2 * i] = ((*P_km1)[im1] * 4.0f + (*P_km1)[i] * 4.f) / 8.0f;
				(*P_k)[2 * i].setRGBA(CYAN);
				(*P_k)[2 * i].pointSize = 5.f;

				(*P_k)[2 * i + 1] = ((*P_km1)[im1] + ((*P_km1)[i] * 6) + (*P_km1)[ip1]) / 8.0f;
				(*P_k)[2 * i + 1].setRGBA(CYAN);
				(*P_k)[2 * i + 1].pointSize = 5.f;
			}

			if (k - 1 != 0){ //If k-1 = 0, then P_km1 is POINTS, which we don't want to delete
				delete P_km1;
			}
			P_km1 = P_k; //Swap over
		}

		if (!secondaryPolygon.isInitialized()){
			secondaryPolygon.setColor(CYAN);
			secondaryPolygon.init(P_k);

			secondaryLinePolygon.setColor(CYAN);
			secondaryLinePolygon.init(P_k);
		}
		else{
			if (ModeLevelChanged){
				secondaryPolygon.init(P_k);
				secondaryLinePolygon.init(P_k);
				ModeLevelChanged = false;
			}
			else{
				secondaryPolygon.update(P_k);
				secondaryLinePolygon.update(P_k);
			}
		}

		//drawPolygon(subdivPoly, true);
		DisplayQueue.push(PolygonQueueItem(secondaryLinePolygon, Lines));
		DisplayQueue.push(PolygonQueueItem(secondaryPolygon, Points));

		delete P_k;
	}
	if (ModeState & MODE_CATMULL){
		unsigned int drawMode = ModeState & 0x01;

		int secondaryPointCount = POINT_COUNT * 3;
		std::vector<Point> & P = *POINTS;

		std::vector<Point> controlPoints(secondaryPointCount); //-1 because the end point = start point

		int plen = P.size();
		for (int i = 0; i < plen; i += 1){
			//Compute the values of i+1 and i-1 with wrap around
			int ip1 = (i + 1) % plen;
			int im1 = (i != 0) ? (i - 1) : (plen - 1); 

			//Compute the difference of P[i+1] - P[i-1] and normalize it
			Vector4f tangent((P[ip1] - P[im1]).XYZW);
			tangent = normalizeVector(tangent);

			//Get the distances between points P[i] and P[i+1], and between P[i] and P[i-1]
			float distanceForward = getPointDistance(P[i], P[ip1]);
			float distanceBackward = getPointDistance(P[i], P[im1]);

			//Compute the indices of the new control points in the new point list. 
			//These are for the control point represeting our current point, P[i], c_i0,
			//  the next control point, c_i1,
			//  and the previous control point, c_(i-1)2
			int currentControlIndex = 3 * i;
			int forwardControlIndex = (3 * i + 1) % (secondaryPointCount - 1);
			int backwardControlIndex = (i != 0)?(3 * i - 1):(secondaryPointCount - 1);

			//Have the position vector for P[i] handy
			Vector4f Pkpos = P[i].XYZW;

			//Position of the next control point, c_i1
			Vector4f forwardControlPosition = { {
				Pkpos[0] + ((0.3f * distanceForward) * tangent[0]), //X
				Pkpos[1] + ((0.3f * distanceForward) * tangent[1]), //Y
				Pkpos[2] + ((0.3f * distanceForward) * tangent[2]), //Z
				1.f
			} };

			//Position of the previous control point, c_(i-1)2
			Vector4f backwardControlPosition = { {
				Pkpos[0] + ((-0.3f * distanceBackward) * tangent[0]), //X
				Pkpos[1] + ((-0.3f * distanceBackward) * tangent[1]), //Y
				Pkpos[2] + ((-0.3f * distanceBackward) * tangent[2]), //Z
				1.f
			} };

			controlPoints[currentControlIndex] = P[i];
			controlPoints[forwardControlIndex] = Point(forwardControlPosition);
			controlPoints[backwardControlIndex] = Point(backwardControlPosition);

			controlPoints[currentControlIndex].pointSize = 3.f;
			controlPoints[forwardControlIndex].pointSize = 3.f;
			controlPoints[backwardControlIndex].pointSize = 3.f;
		}

		if (drawMode == 0){ 
			int pointsPerSegment = 15;
			std::vector<Point> vertices; 
			vertices.reserve(POINT_COUNT * pointsPerSegment);
			
			for (int i = 0; i < POINT_COUNT; i += 1){
				std::vector<Point> segment(4);
				for (int j = 0; j < 4; j += 1){
					segment[j] = controlPoints[(i * 3 + j) % (secondaryPointCount)]; 
				}

				for (int k = 0; k < pointsPerSegment; k += 1){
					float u = (float)k / (float)(pointsPerSegment + 1); //I don't want this to ever hit 1, since it'll overlap with the next segment
					vertices.push_back(deCasteljau(&segment, u));
				}
			}

			if (!secondaryPolygon.isInitialized()){
				secondaryPolygon.setColor(RED);
				secondaryPolygon.init(&controlPoints);

				secondaryLinePolygon.setColor(RED);
				secondaryLinePolygon.init(&vertices);
			}
			else{
				secondaryPolygon.update(&controlPoints);
				secondaryLinePolygon.update(&vertices);
			}
		}
		else if(drawMode == 1){//Drawing just the control points
			if (!secondaryPolygon.isInitialized()){
				secondaryPolygon.setColor(RED);
				secondaryPolygon.init(&controlPoints);

				secondaryLinePolygon.setColor(RED);
				secondaryLinePolygon.init(&controlPoints);
			}
			else{
				secondaryPolygon.update(&controlPoints);
				secondaryLinePolygon.update(&controlPoints);
			}
		}

		//drawPolygon(subdivPoly, true);
		//DisplayQueue.push(PolygonQueueItem(secondaryLinePolygon, Lines));
		//DisplayQueue.push(PolygonQueueItem(secondaryPolygon, Points));
	}
}


float getPointDistance(Point a, Point b){
	float x_a = a.XYZW[0];
	float y_a = a.XYZW[1];

	float x_b = b.XYZW[0];
	float y_b = b.XYZW[1];

	return std::sqrt(std::pow(x_b - x_a, 2.f) + std::pow(y_b - y_a, 2.f));
}

Vector4f normalizeVector(Vector4f input){
	Vector4f returnVector(input);
	float magnitude = std::sqrt(std::pow(input[0], 2) + std::pow(input[1], 2) + std::pow(input[2], 2));

	if (magnitude > 0.000001){
		returnVector[0] /= magnitude;
		returnVector[1] /= magnitude;
		returnVector[2] /= magnitude;
		returnVector[3] /= magnitude;
	}
	return returnVector;
}

//Used algorithm from http://www.cs.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/Bezier/de-casteljau.html
Point deCasteljau(const std::vector<Point> * P, float t){
	if (t < 0 || t > 1){
		throw std::exception();
	}

	std::vector<Point> Q(*P);
	for (int k = 1; k < P->size(); k += 1){
		for (int i = 0; i < (P->size() - k); i += 1){
			Q[i] = Q[i] * (1.f - t) + Q[i + 1] * t;
		}
	}
	return Q[0];
}