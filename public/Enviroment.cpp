#include "Enviroment.h"


#ifdef __cplusplus
extern "C"
{
#endif

#include <SDL/SDL.h>
//#undef main

#ifdef __cplusplus
};
#endif


int init_count = 0;


bool Enviroment::SDLInit()
{
	if (init_count) {
		init_count++;
		return true;
	}

	//SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	init_count++;
	return true;
}


void Enviroment::SDLDestory()
{
	init_count--;
	if(!init_count)
		SDL_Quit();
}
