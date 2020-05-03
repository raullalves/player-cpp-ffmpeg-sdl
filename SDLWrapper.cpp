#include "stdafx.h"
void SDLWrapper::init_sdl()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
		Utils::display_exception("There is something wrong with your SDL Libs. Couldn't run");

#ifdef _WIN32
	SDL_AudioInit("directsound");
#endif
}

void SDLWrapper::open_audio(SDL_AudioSpec* desired, SDL_AudioSpec* obtained)
{
	if (SDL_OpenAudio(desired, obtained) < 0)
		Utils::display_exception("Failed to open audio");
}