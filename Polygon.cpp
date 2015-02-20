#include "Polygon.h"


Polygon::Polygon()
{
}


Polygon::~Polygon()
{
	this->clear();
}

void Polygon::init(std::vector<Point> * const points){
	this->clear(); //Release any other data, if it exists

	this->isInit = true;

	glGenVertexArrays(1, &polygonVAO);
	glBindVertexArray(polygonVAO);

	glGenBuffers(polygonBuffers.size(), &polygonBuffers[0]);

	this->createVertexBuffers(points);

	glBindVertexArray(0); //Unbind the VAO so it's not changed elsewhere
}

void Polygon::createVertexBuffers(std::vector<Point> * const points){
	std::vector<Point> transformedPoints;
	this->pointTransform(points, transformedPoints);
	this->vertexCount = transformedPoints.size();

	std::vector<unsigned int> ids;
	std::vector<Vector4f> positions;
	std::vector<Vector4f> colors;
	std::vector<float> pointSizes;
	std::vector<unsigned int> indices;

	ids.reserve(this->vertexCount);
	positions.reserve(this->vertexCount);
	colors.reserve(this->vertexCount);
	pointSizes.reserve(this->vertexCount);
	indices.reserve(this->vertexCount);

	for (int i = 0; i < transformedPoints.size(); i += 1){
		this->handlePoint(transformedPoints[i], ids, positions, colors, pointSizes);
		indices.push_back(i);
	}

	glBindBuffer(GL_ARRAY_BUFFER, polygonBuffers[ID_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ids[0]) * ids.size(), &ids[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, polygonBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, positions[0].size(), GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, polygonBuffers[COLOR_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors[0]) * colors.size(), &colors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, colors[0].size(), GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, polygonBuffers[POINT_SIZE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointSizes[0]) * pointSizes.size(), &pointSizes[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
	//glVertexAttribIPointer(3, 1, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygonBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

void Polygon::update(std::vector<Point> * const points){
	//this->clear(); //Release any other data, if it exists

	glBindVertexArray(polygonVAO);

	this->updateVertexBuffers(points);

	glBindVertexArray(0); //Unbind the VAO so it's not changed elsewhere
}

void Polygon::updateVertexBuffers(std::vector<Point> * const points){
	std::vector<Point> transformedPoints;
	this->pointTransform(points, transformedPoints);
	this->vertexCount = transformedPoints.size();

	std::vector<unsigned int> ids;
	std::vector<Vector4f> positions;
	std::vector<Vector4f> colors;
	std::vector<float> pointSizes;
	std::vector<unsigned int> indices;

	ids.reserve(this->vertexCount);
	positions.reserve(this->vertexCount);
	colors.reserve(this->vertexCount);
	pointSizes.reserve(this->vertexCount);
	indices.reserve(this->vertexCount);

	for (int i = 0; i < transformedPoints.size(); i += 1){
		this->handlePoint(transformedPoints[i], ids, positions, colors, pointSizes);
		indices.push_back(i);
	}

	glBindBuffer(GL_ARRAY_BUFFER, polygonBuffers[ID_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ids[0]) * ids.size(), &ids[0]);

	glBindBuffer(GL_ARRAY_BUFFER, polygonBuffers[POSITION_VB]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions[0]) * positions.size(), &positions[0]);

	glBindBuffer(GL_ARRAY_BUFFER, polygonBuffers[COLOR_VB]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors[0]) * colors.size(), &colors[0]);

	glBindBuffer(GL_ARRAY_BUFFER, polygonBuffers[POINT_SIZE_VB]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointSizes[0]) * pointSizes.size(), &pointSizes[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygonBuffers[INDEX_VB]);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices[0]) * indices.size(), &indices[0]);
}

/*
 * Do whatever is necessary to take the data from a point, and place it in the data arrays.
 */
void Polygon::handlePoint(const Point point, std::vector<unsigned int> & ids, std::vector<Vector4f> & positions, std::vector<Vector4f> & colors, std::vector<float> & pointSizes){
	ids.push_back(point.id);
	positions.push_back(point.XYZW);
	colors.push_back(point.RGBA);
	pointSizes.push_back(point.pointSize);
}

/*
 * A virtual function for manipulating the input points and outputting them into a new vector. 
 */
void Polygon::pointTransform(std::vector<Point> * const points, std::vector<Point> & newPoints){
	newPoints.reserve(points->size());
	std::copy(points->begin(), points->end(), std::back_inserter(newPoints));
}

void Polygon::render(){
	glBindVertexArray(this->polygonVAO);

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	glDrawElementsBaseVertex(GL_POINTS, this->vertexCount, GL_UNSIGNED_INT, 0, 0);

	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	glBindVertexArray(0);
}

void Polygon::clear(){
	if (this->polygonBuffers[0] != 0) {
		glDeleteBuffers(this->polygonBuffers.size(), &this->polygonBuffers[0]);
	}

	if (this->polygonVAO != 0) {
		glDeleteVertexArrays(1, &this->polygonVAO);
		this->polygonVAO = 0;
	}

	this->isInit = false;
}

bool Polygon::isInitialized(){
	return this->isInit;
}