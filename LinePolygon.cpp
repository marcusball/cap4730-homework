#include "LinePolygon.h"

LinePolygon::LinePolygon()
{
	lineColor = { { 0.f, 0.f, 0.f, 1.f } }; //http://stackoverflow.com/q/6893700/451726
}

LinePolygon::LinePolygon(Vector4f color) : lineColor(color)
{
}


LinePolygon::~LinePolygon()
{
	this->clear();
}


/*
* A virtual function for manipulating the input points and outputting them into a new vector.
*/
void LinePolygon::pointTransform(std::vector<Point> * const points, std::vector<Point> & newPoints){
	newPoints.reserve(points->size() * 2);

	int lsize = newPoints.size();
	for (int i = 0; i < points->size(); i += 1){
		int i2 = (i + 1) % points->size(); //wrap around to beginning to complete

		if (2 * i < lsize){ //If lineVector has already been initialized, and there's room for this item
			newPoints[2 * i] = (*points)[i];
		}
		else{
			newPoints.push_back((*points)[i]);
		}
		newPoints[2 * i].setRGBA(this->lineColor);

		if (2 * i + 1 < lsize){ //If lineVector has already been initialized, and there's room for this item
			newPoints[2 * i + 1] = (*points)[i2];
		}
		else{
			newPoints.push_back((*points)[i2]);
		}
		newPoints[2 * i + 1].setRGBA(this->lineColor);
	}
}

void LinePolygon::render(){
	glBindVertexArray(this->polygonVAO);

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	glDrawElementsBaseVertex(GL_LINES, this->vertexCount, GL_UNSIGNED_INT, 0, 0);

	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	glBindVertexArray(0);
}

void LinePolygon::setColor(Vector4f color){
	this->lineColor = color;
}