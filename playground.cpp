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

struct PickingVariables{
	float m_x;
	float m_y;
	float x_0, x_1;
	float y_0, y_1;
	float y_orig, z_orig, z_delta;

	int selectedView;

	Point * selectedPoint;
};

/** FUNCTION HEADERS **/
const std::vector<Point> getPoints(int numPoints);
void drawPolygon(Polygon & polygon);
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
bool ShiftKeyHeld();
bool DoPicking(GLuint pickingProgramID, glm::mat4 & MVP, PickingVariables & pickingVars, int viewId, unsigned int windowXAxis, unsigned int windowYAxis, unsigned int windowZAxis);
void DoMainRendering(GLuint programID, glm::mat4 & MVP, glm::mat4 & modelMatrix);

/** CONSTANTS **/
const unsigned int POINT_COUNT = 8; // <<================================= POINT COUNT 

const unsigned int RESOLUTION_WIDTH = 1024;
const unsigned int RESOLUTION_HEIGHT = 768;

const float COORD_X_MIN = -4.f;
const float COORD_X_MAX = 4.f;
const float COORD_Y_MIN = -3.f;
const float COORD_Y_MAX = 3.f;

const float CAMERA_EYE_X = 0.f;
const float CAMERA_EYE_Y = 0.f;
const float CAMERA_EYE_Z = -5.f;

const float Z_MOVE_Y_MOD = 2.f; //Multiplier to apply to shift in mouse Y coordinate when [shift]ing a point in Z space.

const unsigned int MOUSE_PRESSED = 1;
const unsigned int MOUSE_HELD = 2;
const unsigned int MOUSE_HAS_CLICKED = MOUSE_PRESSED | MOUSE_HELD;

const unsigned int MODE_SUBDIV = 64;
const unsigned int MODE_CATMULL = 128;
const unsigned int MODE_BEZIER = 256;

const unsigned int DRAW_MODE_POINTS = 0;
const unsigned int DRAW_MODE_LINES = 1;

const unsigned int X_AXIS = 0;
const unsigned int Y_AXIS = 1;
const unsigned int Z_AXIS = 2;

const Vector4f RED = { 1.f, 0.f, 0.f, 1.f };
const Vector4f YELLOW = { 1.f, 1.f, 0.f, 1.f };
const Vector4f GREEN = { 0.f, 1.f, 0.f, 1.f };
const Vector4f CYAN = { 0.f, 1.f, 1.f, 1.f };
const Vector4f BLUE = { 0.f, 0.f, 1.f, 1.f };
const Vector4f PURPLE = { 1.f, 0.f, 1.f, 1.f };
const Vector4f WHITE = { 1.f, 1.f, 1.f, 1.f };

const int SHIFT_KEY_RELEASE_THRESHOLD = 10; //Arbitrary number of frames in which the shift key must be "released" in order to consider it no longer held down

/** GLOBAL VARIABLES **/
GLFWwindow * WINDOW;
std::vector<Point> * POINTS;
std::vector<int> LISTENED_KEYS;
std::vector<int> PRESSED_KEYS; 
std::queue<PolygonQueueItem> DisplayQueue;

glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;

GLuint matrixID;
GLuint pickingMatrixID;
GLuint viewMatrixID;
GLuint modelMatrixID;
GLuint lightID;
GLuint pickingViewID;

Polygon primaryPolygon;
LinePolygon primaryLinePolygon;
Polygon secondaryPolygon;
LinePolygon secondaryLinePolygon;
LinePolygon tertiaryLinePolygon;

unsigned int mouseState = 0; //The current mouse click state; 0 -> no click
unsigned int ModeState = 0;
bool ModeLevelChanged = false;
bool DEBUG_SELECTION_DRAW = false;
bool HideSelectionPoints = false;
int shiftKeyReleaseTime = 0;
bool shiftKeyWasPressed = false;

float scaleFactor = 1.f;

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

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	projectionMatrix = glm::ortho(COORD_X_MIN, COORD_X_MAX, COORD_Y_MIN, COORD_Y_MAX, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	viewMatrix = glm::lookAt(
		glm::vec3(CAMERA_EYE_X, CAMERA_EYE_Y, CAMERA_EYE_Z), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

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

	// Get a handle for our "MVP" uniform
	matrixID = glGetUniformLocation(programID, "MVP");
	viewMatrixID = glGetUniformLocation(programID, "V");
	modelMatrixID = glGetUniformLocation(programID, "M");
	pickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	pickingViewID = glGetUniformLocation(pickingProgramID, "viewId");

	// Get a handle for our "LightPosition" uniform
	lightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	glfwSetInputMode(WINDOW, GLFW_STICKY_KEYS, GL_TRUE);
	

	bool requiresRefresh = true;
	//Just some variables that will be used to calculate the conversion between screen coordinates and vertex coordinates

	PickingVariables pickingVars;
	pickingVars.m_x = -(COORD_X_MAX - COORD_X_MIN) / RESOLUTION_WIDTH;
	pickingVars.m_y = -(COORD_Y_MAX - COORD_Y_MIN) / RESOLUTION_HEIGHT;
	pickingVars.selectedPoint = NULL;

	unsigned int view_x_axis, view_y_axis, view_z_axis;

	scaleFactor = 0.5f;
	do{
		if (requiresRefresh){
			primaryPolygon.update(POINTS);
			primaryLinePolygon.update(POINTS);
			generateSecondaryPoints();

			requiresRefresh = false;
		}

		int viewCount = 2;
		int mode = 2;

		//loop through the draw states of (1) drawing selection points and (2) drawing the actual screen
		for (int drawState = 1; drawState <= 2; drawState += 1){
			switch (drawState){
			case 1: //drawing the selection points
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //blue screen of death
				break;
			case 2: //drawing the actual polygons
				glClearColor(0.0f, 0.0f, 0.4f, 0.0f); //blue screen of death
				break;
			}
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			for (int view = 0; view < viewCount; view += 1){
				glm::mat4 scaleMatrix, translationMatrix, rotationMatrix;
				glm::mat4 modelMatrix;
				glm::mat4 MVP;

				view_x_axis = X_AXIS;
				view_y_axis = Y_AXIS;
				view_z_axis = Z_AXIS;

				if (mode == 1){
					scaleFactor = 1;
					modelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
					MVP = projectionMatrix * viewMatrix * modelMatrix;
				}
				if (mode == 2){
					scaleFactor = 0.5;

					switch (view){
					case 0:
						scaleMatrix = glm::scale(scaleFactor, scaleFactor, scaleFactor);
						translationMatrix = glm::translate(0.0f, 3.5f, 0.0f);
						modelMatrix = glm::mat4(1.0) * scaleMatrix * translationMatrix; // TranslationMatrix * RotationMatrix;
						MVP = projectionMatrix * viewMatrix * modelMatrix;
						break;
					case 1:
						scaleMatrix = glm::scale(scaleFactor, scaleFactor, scaleFactor);
						rotationMatrix = glm::rotate(90.f, glm::vec3(0.f, 1.f, 0.f));
						translationMatrix = glm::translate(0.0f, -3.5f, 0.0f);
						modelMatrix = glm::mat4(1.0) * scaleMatrix * rotationMatrix * translationMatrix; // TranslationMatrix * RotationMatrix;
						MVP = projectionMatrix * viewMatrix * modelMatrix;

						view_x_axis = Z_AXIS;
						view_y_axis = Y_AXIS;
						view_z_axis = X_AXIS;
						break;
					}
				}

				switch (drawState){
				case 1: 
					requiresRefresh |= DoPicking(pickingProgramID, MVP, pickingVars, view, view_x_axis, view_y_axis, view_z_axis);

					glFlush();
					glFinish();
					break;
				case 2:
					DoMainRendering(programID, MVP, modelMatrix);
					break;
				}
			}
		}
		glfwSwapBuffers(WINDOW);

		glfwPollEvents();
		requiresRefresh |= handleModeState(); //Check for input and change the mode state appropriately
	} while (glfwGetKey(WINDOW, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(WINDOW) == 0);
}

void drawPolygon(Polygon & polygon){

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	polygon.render();

	glBlendFunc(GL_NONE, GL_NONE);
	glDisable(GL_BLEND);
}

void drawPolygons(){

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
	float radius = 2.f;

	float rad = ((float)i / (float)n) * 2.0 * M_PI; //Out of a full circle, where should this point be

	p.XYZW[0] = std::cos(rad) * radius;
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
				tertiaryLinePolygon.clear();

				ModeState = MODE_SUBDIV;
				//std::cout << "Setting mode to SUBDIV" << std::endl;
			}
			break;

		case(GLFW_KEY_2):
			secondaryPolygon.clear();
			secondaryLinePolygon.clear();
			tertiaryLinePolygon.clear();

			if (ModeState & MODE_CATMULL){
				int value = ((ModeState & 0x01) + 1) % 2; //We'll allow a max of 2 different subdiv states
				ModeState &= ~0x01; //Clear out the last 4 bits, reset to zero.
				ModeState |= value; //OR in the new value
				ModeLevelChanged = true;
				
			}
			else{
				ModeState = MODE_CATMULL;
			}
			break;
		case(GLFW_KEY_3):
			secondaryPolygon.clear();
			secondaryLinePolygon.clear();
			tertiaryLinePolygon.clear();

			ModeState = MODE_BEZIER;
			//std::cout << "Setting mode to BEZIER" << std::endl;
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
		if (subdivLevel == 0){
			secondaryPolygon.clear();
			secondaryLinePolygon.clear();
			return;
		}

		float pointSize = 5.f;
		if (subdivLevel >= 4){
			pointSize = std::max(5.f - (subdivLevel - 3),1.f);
		}

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
				(*P_k)[2 * i].pointSize = pointSize;

				(*P_k)[2 * i + 1] = ((*P_km1)[im1] + ((*P_km1)[i] * 6) + (*P_km1)[ip1]) / 8.0f;
				(*P_k)[2 * i + 1].setRGBA(CYAN);
				(*P_k)[2 * i + 1].pointSize = pointSize;
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

		std::vector<Point> controlPoints(secondaryPointCount); 

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

		//This is the part where we evaluate the control points to get the curve vertices
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

			secondaryLinePolygon.setColor(GREEN);
			secondaryLinePolygon.init(&vertices);

			
		}
		else{
			secondaryPolygon.update(&controlPoints);
			secondaryLinePolygon.update(&vertices);
			
		}

		//I added in a second draw mode to hide the control lines
		if (drawMode == 1){
			tertiaryLinePolygon.clear();
		}
		else{
			if (!tertiaryLinePolygon.isInitialized()){
				tertiaryLinePolygon.setColor(RED);
				tertiaryLinePolygon.init(&controlPoints);
			}
			else{
				tertiaryLinePolygon.update(&controlPoints);
			}
		}
	}
	if (ModeState & MODE_BEZIER){
		std::vector<Point> & P = *POINTS;

		std::vector<std::array<Point, 4>> coefficients(P.size());

		//Calculate all of the coefficients
		for (int i = 0; i < P.size(); i += 1){
			int ip1 = (i + 1) % P.size();
			int ip2 = (i + 2) % P.size();
			coefficients[i][1] = (P[i] * 2.f + P[ip1]) / 3.f;
			coefficients[i][2] = (P[i] * 1.f + P[ip1] * 2.f) / 3.f;
			coefficients[i][3] = (((P[ip1] * 2.f + P[ip2]) / 3.f) + coefficients[i][2]) / 2.f;
			coefficients[ip1][0] = coefficients[i][3];
		}

		//Copy the coefficients as control points
		std::vector<Point> controlPoints(P.size() * 3);
		for (int i = 0; i < P.size(); i += 1){
			for (int j = 0; j < 3; j += 1){
				controlPoints[i * 3 + j] = coefficients[i][j];
				controlPoints[i * 3 + j].pointSize = 4.f;
			}
		}

		//Evaluate the control points using deCasteljau's algorithm
		int pointsPerSegment = 15;
		std::vector<Point> vertices;
		vertices.reserve(POINT_COUNT * pointsPerSegment);

		for (int i = 0; i < POINT_COUNT; i += 1){
			for (int k = 0; k < pointsPerSegment; k += 1){
				float u = (float)k / (float)(pointsPerSegment + 1); //I don't want this to ever hit 1, since it'll overlap with the next segment
				std::vector<Point> segment(coefficients[i].begin(), coefficients[i].end());
				vertices.push_back(deCasteljau(&segment, u));
			}
		}

		if (!secondaryPolygon.isInitialized()){
			secondaryPolygon.setColor(YELLOW);
			secondaryPolygon.init(&controlPoints);

			secondaryLinePolygon.setColor(YELLOW);
			secondaryLinePolygon.init(&vertices);
		}
		else{
			secondaryPolygon.update(&controlPoints);
			secondaryLinePolygon.update(&vertices);
		}
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

bool ShiftKeyHeld(){
	if (glfwGetKey(WINDOW, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS || glfwGetKey(WINDOW, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
		shiftKeyWasPressed = true;
		shiftKeyReleaseTime = 0;
		return true;
	}
	else{
		if (shiftKeyWasPressed){
			if (shiftKeyReleaseTime < SHIFT_KEY_RELEASE_THRESHOLD){
				shiftKeyReleaseTime += 1;
				return true;
			}
			else{
				std::cout << "releasing shift key";
				shiftKeyReleaseTime = 0;
				shiftKeyWasPressed = false;
				return false;
			}
		}
		else{
			return false;
		}
	}
}

bool DoPicking(GLuint pickingProgramID, glm::mat4 & MVP, PickingVariables & pickingVars, int viewId, unsigned int windowXAxis, unsigned int windowYAxis, unsigned int windowZAxis){
	bool requiresRefresh = false;
	
	// PICKING IS DONE HERE
	if (glfwGetMouseButton(WINDOW, GLFW_MOUSE_BUTTON_LEFT)){
		mouseState |= MOUSE_PRESSED;

		glUseProgram(pickingProgramID);
		{
			// Send our transformation to the currently bound shader, in the "MVP" uniform
			glUniformMatrix4fv(pickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniform1ui(pickingViewID, viewId);


			//HiddenDisplayQueue.push(VaoItem(pointBuffer, pointBufferArray, POINT_COUNT, DRAW_MODE_POINTS));

			drawPolygon(primaryPolygon);

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
			int pickedView = int(data[2]);

			if (pickedView == viewId || pickingVars.selectedView == viewId){

				if (mouseState & MOUSE_HELD){ //If the mouse has already been held down for some amount of time...
					if (pickingVars.selectedPoint != NULL){ //Make sure we actually got a selected point, and it's not a null pointer
						if (!ShiftKeyHeld()){
							float x_n = (1.f / scaleFactor) * pickingVars.m_x * (xpos - pickingVars.x_0) + pickingVars.x_1; //Calculate the new point position; map (0 - screen_width) -> (-1 - 1)
							float y_n = (1.f / scaleFactor) * pickingVars.m_y * (ypos - pickingVars.y_0) + pickingVars.y_1; //Calculate new Y point position; map (0 - screen_height) -> (-1 - 1)

							//Update the position
							pickingVars.selectedPoint->XYZW[windowXAxis] = x_n;
							pickingVars.selectedPoint->XYZW[windowYAxis] = y_n;

							//Reset the Z tracking values so they're up to date if the user suddenly presses shift
							pickingVars.y_orig = pickingVars.selectedPoint->XYZW[windowYAxis];
							pickingVars.z_orig = pickingVars.selectedPoint->XYZW[windowZAxis];
							pickingVars.z_delta = 0.f;
						}
						else{
							float y_n = (1.f / scaleFactor) * Z_MOVE_Y_MOD * pickingVars.m_y * (ypos - pickingVars.y_0) + pickingVars.y_1; //Calculate new Y point position; map (0 - screen_height) -> (-1 - 1)

							pickingVars.z_delta = y_n - pickingVars.y_orig;

							pickingVars.selectedPoint->XYZW[windowZAxis] = std::max(pickingVars.z_orig + pickingVars.z_delta, CAMERA_EYE_Z);
						}

						std::printf("Position of point %d is now <%.1f, %.1f, %.1f>\n", pickingVars.selectedPoint->id, pickingVars.selectedPoint->XYZW[0], pickingVars.selectedPoint->XYZW[1], pickingVars.selectedPoint->XYZW[2]);
					}
				}
				else if (mouseState & MOUSE_PRESSED){ //If the mouse just now got clicked for the first time (eg. click begins)
					if (pickedID != 255){ //Not the background
						pickingVars.selectedPoint = getPointById(pickedID); //get a pointer to the selected Point object
						if (pickingVars.selectedPoint != NULL){ //Make sure something didn't go wrong
							pickingVars.selectedPoint->setRGBA(RED);
							pickingVars.selectedPoint->pointSize *= 2.f;
							//selectedPoint->invertColor();

							pickingVars.selectedView = pickedView;

							//Update the reference points so our conversion math works in the next states (see above).
							pickingVars.x_0 = xpos;
							pickingVars.x_1 = pickingVars.selectedPoint->XYZW[windowXAxis];

							pickingVars.y_0 = ypos;
							pickingVars.y_1 = pickingVars.selectedPoint->XYZW[windowYAxis];

							pickingVars.y_orig = pickingVars.selectedPoint->XYZW[windowYAxis];
							pickingVars.z_orig = pickingVars.selectedPoint->XYZW[windowZAxis];
							pickingVars.z_delta = 0.f;
						}

						//Update the mouse state
						mouseState = MOUSE_HELD;
					}
					else{
						if (pickingVars.selectedPoint != NULL){
							//Reset the Z tracking values so they're up to date if the user suddenly presses shift
							pickingVars.y_orig = pickingVars.selectedPoint->XYZW[windowYAxis];
							pickingVars.z_orig = pickingVars.selectedPoint->XYZW[windowZAxis];
							pickingVars.z_delta = 0.f;
						}
					}
				}
				requiresRefresh = true;
			}
		}
	}
	else{ //The mouse is not being clicked
		if (mouseState & MOUSE_HAS_CLICKED){ //but the mouse WAS clicked before (so it's been released).
			if (pickingVars.selectedPoint != NULL){
				//selectedPoint->setRGBA(1.f, 0.f, 0.f, 1.f); //Return the point to its original color.
				//selectedPoint->invertColor();
				pickingVars.selectedPoint->setRGBA(WHITE);
				pickingVars.selectedPoint->pointSize /= 2.f;
			}

			pickingVars.selectedPoint = NULL; //clear it, just to be safe
			pickingVars.selectedView = 0;
			mouseState = 0; //Return the mouse state to 0 (no click events present)
			requiresRefresh = true;
		}
	}
	return requiresRefresh;
}

void DoMainRendering(GLuint programID, glm::mat4 & MVP, glm::mat4 & modelMatrix){

	glUseProgram(programID); //Use the normal shaders
	{
		//glfwSwapInterval(1);

		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);
		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);

		DisplayQueue.push(PolygonQueueItem(tertiaryLinePolygon, Lines));

		DisplayQueue.push(PolygonQueueItem(secondaryLinePolygon, Lines));
		DisplayQueue.push(PolygonQueueItem(secondaryPolygon, Points));

		if (!HideSelectionPoints){
			DisplayQueue.push(PolygonQueueItem(primaryLinePolygon, Lines));
			DisplayQueue.push(PolygonQueueItem(primaryPolygon, Points));
		}

		drawPolygons();
	}
}