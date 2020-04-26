#include "stdafx.h"

#ifdef main
#undef main
#endif

using namespace std;

int main(int argc, const char *argv[]) {

	if(argc != 2){
		cout << "Usage: ./player <video_addr>"<<endl;
		exit(-1);
	}

	cout << "checked for argc" << endl;
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		cout << "There is something wrong with your SDL Libs. Can't run" << endl;
		exit(-1);
	}

	Player * player = new Player(argv[1]);
	
	int res = player->alocarMemoria();
	if (res < 0) {
		cout << "Fatal Error";
		delete(player);
		exit(-1);
	}

	cout << "display gonna be created" << endl;

	res= player->criarDisplay();

	cout << "Frames gonna be read" << endl;

	res = player->lerFramesVideo();
	if (res < 0) {
		cout << "Shit happens" << endl;
		delete(player);
		exit(-1);
	}

	cout << "Finished video" << endl;

	delete(player);

	return 0;
}