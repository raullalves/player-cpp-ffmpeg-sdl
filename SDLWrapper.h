#pragma once
class SDLWrapper
{
public:
	static void init_sdl();
	static void open_audio(SDL_AudioSpec*, SDL_AudioSpec*);

private:
	SDLWrapper() {};
};