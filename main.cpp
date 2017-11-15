#include <iostream>
#include <string>
#include <chrono>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "Player.hpp"

//SDL define main. IMPORTANTE!
#ifdef main
#undef main
#endif

using namespace std;
using namespace cv;
using namespace chrono;

int main(int argc, const char *argv[]) {

	if(argc != 2){
		cout << "Usage: ./player video_addr"<<endl;
		exit(-1);
	}
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		cout << "There is something wrong with your SDL Libs. Can't run" << endl;
		exit(-1);
	}
	
	Player * player = new Player(argv[1]);
	
	int res = player->alocarMemoria();
	if (res < 0) {
		cout << "Fatal Error";
		exit(-1);
	}

	res= player->criarDisplay();
	
	res = player->lerFramesVideo();
	if (res < 0) {
		cout << "Shit happens" << endl;
		exit(-1);
	}

	return 0;
}