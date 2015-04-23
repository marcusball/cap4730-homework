#pragma once
#include "Game.h"

/***************************************************/
/** Program entry-point and main graphics loop    **/
/***************************************************/
int main(){
	Game * game = Game::GetInstance();
	return game->Run();
}