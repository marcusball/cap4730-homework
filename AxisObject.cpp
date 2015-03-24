#include "AxisObject.h"


AxisObject::AxisObject(){

}


AxisObject::~AxisObject()
{
}

void AxisObject::Init(float lineLength){
	this->Clear();
	this->isInit = true;

	glGenVertexArrays(1, &objectVAO);
	glBindVertexArray(objectVAO);

	glGenBuffers(objectBuffers.size(), &objectBuffers[0]);

	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;
	this->GenerateVertices(lineLength, indices, vertices);
	this->CreateVertexBuffers(&vertices, &indices);

	glBindVertexArray(0); //Unbind the VAO so it's not changed elsewhere
}

void AxisObject::GenerateVertices(float lineLength, std::vector<unsigned int> & outIndices, std::vector<Vertex> & outVertices){
	outVertices.resize(6);

	outVertices[0] = Vertex(Vector4f(0.f, 0.f, 0.f, 1.f), Vector3f(0.f, 0.f, 1.f), ColorVectors::RED);
	outVertices[1] = Vertex(Vector4f(lineLength, 0.f, 0.f, 1.f), Vector3f(0.f, 0.f, 1.f), ColorVectors::RED);

	outVertices[2] = Vertex(Vector4f(0.f, 0.f, 0.f, 1.f), Vector3f(0.f, 0.f, 1.f), ColorVectors::GREEN);
	outVertices[3] = Vertex(Vector4f(0.f, lineLength, 0.f, 1.f), Vector3f(0.f, 0.f, 1.f), ColorVectors::GREEN);

	outVertices[4] = Vertex(Vector4f(0.f, 0.f, 0.f, 1.f), Vector3f(0.f, 0.f, 1.f), ColorVectors::BLUE);
	outVertices[5] = Vertex(Vector4f(0.f, 0.f, lineLength, 1.f), Vector3f(0.f, 0.f, 1.f), ColorVectors::BLUE);

	outIndices = { { 0, 1, 2, 3, 4, 5 } };
}


void AxisObject::Render(){
	glBindVertexArray(this->objectVAO);

	glEnable(GL_POINT_SMOOTH);
	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	glDrawElementsBaseVertex(GL_LINES, 6, GL_UNSIGNED_INT, 0, 0);

	glDisable(GL_POINT_SMOOTH);
	//glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	glBindVertexArray(0);
}
