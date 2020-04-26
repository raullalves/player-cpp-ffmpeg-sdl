#include "stdafx.h"

#ifdef main
#undef main
#endif

int main(int argc, const char *argv[]) {
	const char* video_addr = "D:/video.mp4";
	/*if(argc != 2){
		cout << "Usage: ./player <video_addr>"<<endl;
		exit(-1);
	}*/

	SDLWrapper::init_sdl();
	Player::get_instance()->run(video_addr);
	Player::get_instance()->clear();
}