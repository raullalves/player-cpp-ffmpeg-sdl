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

	//Player * player = new Player(argv[1]);
	Player * player = new Player(video_addr);
	
	int res = player->malloc();
	if (res < 0) {
		std::cout << "Error allocating memory";
		delete(player);
		return -1;
	}

	res= player->create_display();

	res = player->display_video();
	if (res < 0) {
		std::cout << "Shit happens" << std::endl;
		delete(player);
		return -1;
	}

	delete(player);

	return 0;
}