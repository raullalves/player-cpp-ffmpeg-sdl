#include "Player.hpp"

using namespace std;
using namespace cv;
//construtor
Player::Player(string endereco) {

	//inicializa��o do ffmpeg
	av_register_all();

	//abre o v�deo
	int res = avformat_open_input(&pFormatCtx, endereco.c_str(), NULL, NULL);

	// Abrir o arquivo de v�deo
	if (res!=0){
		exibirErro(res);
		return;
	}

	//obtem informa��o sobre o stream
	res = avformat_find_stream_info(pFormatCtx, NULL);
	if (res < 0) {
		exibirErro(res);
		return;
	}

	//obtem o stream do video
	videoStream = obterCodecParameters();
	if (videoStream == -1) {
		cout << "Erro ao obter o stream do video em AVCodecParameters, nao tem codecpar_type do tipo AVMEDIA_TYPE_VIDEO" << endl;
		return;
	}

	if (lerCodecVideo() < 0) return;
		

}

Player::~Player(void) {

	av_free(buffer);
	av_free(pFrameRGB);

	// Free the YUV frame
	av_free(pFrame);

	// Close the codecs
	avcodec_close(pCodecCtx);

	// Close the video file
	avformat_close_input(&pFormatCtx);

}

//exibe informa��o dos streams do arquivo de v�deo espec�fico
void Player::exibirInformacaoArquivoVideo(void) {

	av_dump_format(pFormatCtx, 0, pFormatCtx->filename, 0);

}


//exibe o erro, descrevendo-o
void Player::exibirErro(int erro) {

	char errobuf[ERROR_SIZE];
	av_strerror(erro, errobuf, ERROR_SIZE);
	cout << "Erro = " << errobuf<<endl;

}

//obtem o stream do video
int Player::obterCodecParameters(void) {


	int videoStream = -1;
	for (int i = 0; i<pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	if (videoStream == -1) {
		cout << "Nao achou o stream" << endl;
		return -1;
	}
	pCodecParameters = pFormatCtx->streams[videoStream]->codecpar;

	return videoStream;

}

//ler o codec do stream em quest�o, do v�deo lido
int Player::lerCodecVideo(void) {

	//v� se o codec � suportado
	pCodec = avcodec_find_decoder(pCodecParameters->codec_id);

	if (pCodec == NULL) {
		cout << " codec nao suportado " << endl;
		return -1; // Codec not found
	}

	//cria uma estrutura AVCodecContext alocada com o codec em quest�o
	pCodecCtx = avcodec_alloc_context3(pCodec);

	//copia os par�metros originais do v�deo de pCodecParameters para o codecContext
	int res = avcodec_parameters_to_context(pCodecCtx, pCodecParameters);

	if (res < 0) {

		avformat_close_input(&pFormatCtx);
		avcodec_free_context(&pCodecCtx);
		exibirErro(res);
		return res;

	}

	//executa o codec
	res = avcodec_open2(pCodecCtx, pCodec, NULL);
	if (res < 0) {

		exibirErro(res);
		return res; 

	}
	return 1;
}

//aloca a mem�ria de acordo com o tamanho do v�deo
int Player::alocarMemoria(void) {

	pFrame = av_frame_alloc();
	if (pFrame == NULL) {
		cout << "N�o foi possivel alocar a memoria do frame RGB" << endl;
		return -1;
	}

	pFrameRGB = av_frame_alloc();
	if (pFrame == NULL) {
		cout << "N�o foi possivel alocar a memoria do frame RGB" << endl;
		return -1;
	}

	//ver essa quest�o do formato escolhido RGB24 ou YUV420p
	int numBytes = av_image_get_buffer_size(FORMATO, pCodecCtx->width, pCodecCtx->height,1);
	cout << "qtd bytes="<<numBytes << endl;

	//cria a mem�ria necess�ria para o buffer
	buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	//associa o buffer ao Frame
	int res = av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, FORMATO, pCodecCtx->width, pCodecCtx->height, 1);
	if (res < 0) {
		exibirErro(res);
		return res;
	}
	return 1;
}

//L� a "data" que vem do stream de v�deo
int Player::lerFramesVideo(void) {

	AVPacket packet;

	sws_ctx = sws_getContext(pCodecCtx->width,
		pCodecCtx->height,
		pCodecCtx->pix_fmt,
		pCodecCtx->width,
		pCodecCtx->height,
		FORMATO,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL
		);

	int frameFinished = -1;

	//lendo e colocando no packet
	while (av_read_frame(pFormatCtx, &packet) >= 0) {

		//verifica��o se foi transmitido pelo Stream do Context
		if (packet.stream_index == videoStream) {

			//processo de decodifica��o
			int res = avcodec_send_packet(pCodecCtx, &packet);
			if (res < 0) {
				exibirErro(res);
				continue;
			}

			res = avcodec_receive_frame(pCodecCtx, pFrame);
			if (res < 0) {
				exibirErro(res);
				continue;
			}

			//convertendo para RGB
			if (frameFinished) {
				cout << "Executando" << endl;
				SDL_UpdateYUVTexture(bmp, NULL, pFrame->data[0], pFrame->linesize[0],
					pFrame->data[1], pFrame->linesize[1],
					pFrame->data[2], pFrame->linesize[2]);
				SDL_RenderCopy(renderer, bmp, NULL, NULL);
				SDL_RenderPresent(renderer);
				SDL_UpdateWindowSurface(screen);
				SDL_Delay(1000/30);
			}

		}
		//ver depois se � mesmo necess�rio
		av_packet_unref(&packet);
	}

	return 1;

}

int Player::criarDisplay(void) {

	screen = SDL_CreateWindow("Video Player teste",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		pCodecCtx->width, pCodecCtx->height,
		SDL_WINDOW_OPENGL);

	if (!screen) {
		cout << "N�o foi possivel setar a screen com o v�deo desse tamannho" << endl;
		return -1;
	}

	renderer = SDL_CreateRenderer(screen, -1, 0);

	bmp = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STATIC, pCodecCtx->width, pCodecCtx->height);

	return 1;
}