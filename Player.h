#include "stdafx.h"

typedef struct _AudioPacket
	{
		AVPacketList *first, *last;
		int nb_packets, size;
  		SDL_mutex *mutex;
  		SDL_cond *cond;
	} AudioPacket;

class Player {

public:
	
	//construtor
	Player(std::string endereco) {

		audioStream = -1;

		//open video
		int res = avformat_open_input(&pFormatCtx, endereco.c_str(), NULL, NULL);

		//check video opened
		if (res!=0){
			exibirErro(res);
			exit(-1);
		}

		//get video info
		res = avformat_find_stream_info(pFormatCtx, NULL);
		if (res < 0) {
			exibirErro(res);
			exit(-1);
		}

		//get video stream
		videoStream = obterCodecParameters();
		if (videoStream == -1) {
			std::cout << "Error opening your video using AVCodecParameters, does not have codecpar_type type AVMEDIA_TYPE_VIDEO" << std::endl;
			exit(-1);
		}

		if (lerCodecVideo() < 0) exit(-1);

	}

	~Player(void) {

		av_free(buffer);
		av_free(pFrameRGB);

		// Free the YUV frame
		av_free(pFrame);

		// Close the codecs
		avcodec_close(pCodecCtx);

		// Close the video file
		avformat_close_input(&pFormatCtx);

	}

	
	int malloc(void);
	int display_video(void);
	int create_display(void);
	
	static int getAudioPacket(AudioPacket*, AVPacket*, int);

private:
	
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
	
	//exibe o erro com rela��o ao seu respectivo c�digo
	void exibirErro(int erro);

	int obterCodecParameters(void);

	int lerCodecVideo(void);

	int PacketQueuePut(AudioPacket *, const AVPacket *);

	void initAudioPacket(AudioPacket *); 

	int putAudioPacket(AudioPacket *, AVPacket *); 

};