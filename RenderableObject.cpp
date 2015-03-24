#include "RenderableObject.h"

RenderableObject::RenderableObject(){

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