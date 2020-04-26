#include "stdafx.h"

class Player 
{

public:
	static Player* get_instance();

	void run(std::string video_addr)
	{
		this->video_addr = video_addr;

		this->open();
		this->malloc();
		this->create_display();
		this->display_video();
	}

	static int getAudioPacket(AudioPacket*, AVPacket*, int);
	void clear();
private:
	
	static Player* instance;
	Player() {}

	std::string video_addr;
	void open();

	int malloc(void);
	int display_video(void);
	int create_display(void);

	void memsetAudioPacket(AudioPacket * pq);
	//armazena o �ndice do determinado Stream a ser transmitido
	int videoStream;

	//stream de audio
	int audioStream;

	//contem informa��es sobre o arquivo de v�deo, incluindo os codecs, etc
	AVFormatContext *pFormatCtx = NULL;

	//contem informa��es do codec do v�deo, obtidas atraves de
	//pFormatCtx->streams[i]->codecpar
	//olhando o codec_type e vendo se � transmissao de video do tipo AVMEDIA_TYPE_VIDEO
	AVCodecParameters *pCodecParameters = NULL;

	//Audio COdec Parametrs
	AVCodecParameters *pCodecAudioParameters = NULL;

	//informa��es do codecParameters, por�m copiadas. o pCodecParameters serve como um backup das informa��es do v�deo
	AVCodecContext *pCodecCtx = NULL;

	AVCodecContext *pCodecAudioCtx = NULL;

	SDL_AudioSpec wantedSpec = { 0 }, audioSpec = { 0 };

	//guarda o codec do v�deo
	AVCodec *pCodec = NULL;

	//guarda o codec do audio
	AVCodec *pAudioCodec = NULL;

	//estrutura que guarda o frame
	AVFrame *pFrame = NULL;

	//estrutura que guarda o frame RGB
	AVFrame *pFrameRGB = NULL;

	//buffer para leitura dos frames
	uint8_t *buffer = NULL;

	//estrutura que armazena a convers�o para RGB
	struct SwsContext *sws_ctx = NULL;

	//surface window para exibir o video
	//pode ter m�ltiplas screens
	SDL_Window *screen;

	SDL_Renderer *renderer;

	SDL_Texture* bmp;

	int get_video_stream(void);

	int read_audio_video_codec(void);

	int PacketQueuePut(AudioPacket *, const AVPacket *);

	void initAudioPacket(AudioPacket *); 

	int putAudioPacket(AudioPacket *, AVPacket *); 

};