#include "stdafx.h"
void SDLWrapper::init_sdl()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		throw std::exception("There is something wrong with your SDL Libs. Couldn't run");
	}
}