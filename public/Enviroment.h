#ifndef H_SDL_ENVIROMENT
#define H_SDL_ENVIROMENT

#include <SDL/SDL.h>
#undef main


class Enviroment
{
public:
	virtual ~Enviroment() {};

public:
	static bool SDLInit();
	static void SDLDestory();

};


#endif //H_SDL_ENVIROMENT
