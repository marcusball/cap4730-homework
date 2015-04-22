#include "RenderableObject.h"

RenderableObject::RenderableObject() : objectVAO(0){

}

RenderableObject::~RenderableObject(){
	this->Clear();
}

void RenderableObject::Clear(){
	//If the first buffer is not empty, then we can assume all are in use.
	//We'll go ahead and delete all of those buffers.
	if (this->objectBuffers[0] != 0) {
		glDeleteBuffers(this->objectBuffers.size(), &this->objectBuffers[0]);
	}

	//Delete the VAO if it's present.
	if (this->objectVAO != 0) {
		glDeleteVertexArrays(1, &this->objectVAO);
		this->objectVAO = 0; //Reset the VAO variable itself.
	}

	//Revert the object back to an uninitialized state
	this->isInit = false;
}

void RenderableObject::CreateVertexBuffers(const std::vector<Vertex> * const vertices, const std::vector<unsigned int> * const indices){
	//// Create the VAO
	//glGenVertexArrays(1, &this->objectVAO);
	//glBindVertexArray(this->objectVAO);

	//// Create the buffers for the vertices atttributes
	//glGenBuffers(this->objectBuffers.size(), &this->objectBuffers[0]);


	std::vector<Vector4f> positions;
	std::vector<Vector4f> colors;
	std::vector<Vector4f> normals;
	std::vector<float> sizes;

	positions.reserve(vertices->size());
	normals.reserve(vertices->size());
	sizes.reserve(vertices->size());
	for (int x = 0; x < vertices->size(); x += 1){
		positions.push_back((*vertices)[x].Position);
		colors.push_back((*vertices)[x].Color);
		normals.push_back((*vertices)[x].Normal);
		sizes.push_back((*vertices)[x].Size);
	}

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, positions[0].value.size(), GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[COLOR_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors[0]) * colors.size(), &colors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, colors[0].value.size(), GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[SIZE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sizes[0]) * sizes.size(), &sizes[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, normals[0].value.size(), GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->objectBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof((*indices)[0]) * indices->size(), &(*indices)[0], GL_STATIC_DRAW);
}