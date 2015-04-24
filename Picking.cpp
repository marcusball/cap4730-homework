#include "Picking.h"
#include "Game.h"

GLuint Picking::FBO = 0;
GLuint Picking::idTexture = 0;
GLuint Picking::depthTexture = 0;

bool Picking::Initialize(){
	//Get our game object instance
	Game * game = Game::GetInstance();

	//Generate FBO
	glGenFramebuffers(1, &Picking::FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, Picking::FBO);

	//Generate the texture to be used for the id buffer.
	glGenTextures(1, &Picking::idTexture);
	glBindTexture(GL_TEXTURE_2D, Picking::idTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, game->RESOLUTION_WIDTH, game->RESOLUTION_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Picking::idTexture, 0);

	//Generate the texture for the depth buffer
	glGenTextures(1, &Picking::depthTexture);
	glBindTexture(GL_TEXTURE_2D, Picking::depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, game->RESOLUTION_WIDTH, game->RESOLUTION_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Picking::depthTexture, 0);

	//Tutorial says this is a good idea
	glReadBuffer(GL_NONE);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE){
		printf("[*] Fuckin buffer rats, chewin through me framebuffer machiens\n");
		return false;
	}

	//Restore indepence to the tiny republic
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void Picking::Enable(){
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Picking::FBO);
}

void Picking::Disable(){
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}


PixelData Picking::ReadPixelAt(Vector2d coord){
	glBindFramebuffer(GL_READ_FRAMEBUFFER, Picking::FBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	PixelData pixel;
	glReadPixels(coord[0], coord[1], 1, 1, GL_RGB, GL_FLOAT, &pixel);

	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	return pixel;
}

void Picking::UpdateObjectId(GLuint objectIdLocation, unsigned int objectId){
	glUniform1ui(objectIdLocation, objectId);
}