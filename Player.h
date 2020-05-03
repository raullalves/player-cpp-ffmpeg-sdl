#include "stdafx.h"

class Player 
{

public:
	static Player* get_instance();

	void run(std::string, std::string window_name="");
	void clear();

	static int getAudioPacket(AudioPacket*, AVPacket*, int);

private:
	static Player* instance;
	Player() {}

	void open();

	int malloc(void);
	int display_video(void);
	int create_display(void);

	int get_video_stream(void);

	int read_audio_video_codec(void);

	std::string video_addr;
	std::string window_name;

	int videoStream = 0;
	int audioStream = 0;

	AVFormatContext* pFormatCtx = NULL;

	AVCodecParameters* pCodecParameters = NULL;
	AVCodecParameters* pCodecAudioParameters = NULL;

	AVCodecContext* pCodecCtx = NULL;
	AVCodecContext* pCodecAudioCtx = NULL;

	AVCodec* pCodec = NULL;
	AVCodec* pAudioCodec = NULL;
	AVFrame* pFrame = NULL;
	AVFrame* pFrameRGB = NULL;

	uint8_t* buffer = NULL;

	SDL_Window* screen = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* bmp = NULL;

	struct SwsContext* sws_ctx = NULL;
};