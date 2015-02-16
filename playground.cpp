#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <time.h>

#include <GL/glew.h> //Apparently this needs to be before gl.h, and glfw.h. Queen bee of the OpenGL Plastics.
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "common\shader.hpp"
#include "Point.h"

/** FUNCTION HEADERS **/
const std::vector<Point> getPoints(int numPoints);
GLuint makePointVao(const Point * g_vertex_buffer_data, int numPoints);
GLuint createIndexBuffer(int numPoints);
void display(GLuint & vao);
void fakeDraw(GLuint & vao);
float randFloat(float min, float max);
unsigned int getPointIndexById(int searchId);
Point * const getPointById(int searchId);

/** CONSTANTS **/
const unsigned int POINT_COUNT = 20;
const unsigned int VERTEX_DIMENSIONS = 4; // 3D space
const unsigned int RESOLUTION_WIDTH = 1024;
const unsigned int RESOLUTION_HEIGHT = 768;

const unsigned int MOUSE_PRESSED = 1;
const unsigned int MOUSE_HELD = 2;
const unsigned int MOUSE_HAS_CLICKED = MOUSE_PRESSED | MOUSE_HELD;

/** GLOBAL VARIABLES **/
GLFWwindow * WINDOW;
std::vector<Point> * POINTS;

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
	
	unsigned int mouseState = 0; //The current mouse click state; 0 -> no click
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
						}
					}
					else if (mouseState & MOUSE_PRESSED){ //If the mouse just now got clicked for the first time (eg. click begins)
						selectedPoint = getPointById(pickedID); //get a pointer to the selected Point object
						if (selectedPoint != NULL){ //Make sure something didn't go wrong
							//selectedPoint->setRGBA(0.f, 1.f, 0.f, 1.f);
							selectedPoint->invertColor();

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
					selectedPoint->invertColor();
				}

				selectedPoint = NULL; //clear it, just to be safe
				mouseState = 0; //Return the mouse state to 0 (no click events present)
			}
		}


		triangleBuffer = makePointVao(pointVerticies, POINT_COUNT); // Refresh the vertices

		glUseProgram(programID); //Use the normal shaders
		display(triangleBuffer); //Draw the boxes
		glfwPollEvents();
	} while (glfwGetKey(WINDOW, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(WINDOW) == 0);
}

/* 
 * Draw the boxes to the screen!
 */
void display(GLuint & vao){
	static const size_t pointIdOffset = 0;
	static const size_t pointVertexOffset = pointIdOffset + sizeof((*POINTS)[0].id);
	static const size_t pointColorOffset = pointVertexOffset + sizeof((*POINTS)[0].XYZW);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f); //blue screen of death
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(20.0);

	glBindBuffer(GL_ARRAY_BUFFER, vao);
	glVertexAttribPointer(0, VERTEX_DIMENSIONS, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)pointVertexOffset); //Verticies 
	glVertexAttribPointer(1, VERTEX_DIMENSIONS, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)pointColorOffset); //Colors

	glDrawArrays(GL_POINTS, 0, POINT_COUNT * sizeof(Point));

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
	glPointSize(20.0);

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
	float RED[4] = { 1.f, 0.f, 0.f, 1.f };
	for (int i = 0; i < numPoints; i += 1){
		Point p(i);
		p.XYZW[0] = randFloat(-1.f, 1.f);
		p.XYZW[1] = randFloat(-1.f, 1.f);
		p.XYZW[2] = 0.f;
		p.XYZW[3] = 1.f;

		//p.setRGBA(RED);
		p.RGBA[0] = randFloat(0.f, 1.f);
		p.RGBA[1] = randFloat(0.f, 1.f);
		p.RGBA[2] = randFloat(0.f, 1.f);

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