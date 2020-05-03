#include "stdafx.h"

using namespace std;

Player* Player::instance = 0;
Player* Player::get_instance()
{
	if (instance == 0)
		instance = new Player();
	return instance;
}

void Player::run(std::string video_addr, std::string window_name)
{
	this->video_addr = video_addr;
	this->window_name = window_name;

	this->open();
	this->malloc();
	this->create_display();
	this->display_video();
}

void Player::open()
{
	audioStream = -1;

	// open video
	int res = avformat_open_input(&pFormatCtx, this->video_addr.c_str(), NULL, NULL);

	// check video
	if (res != 0)
		Utils::display_ffmpeg_exception(res);

	// get video info
	res = avformat_find_stream_info(pFormatCtx, NULL);
	if (res < 0)
		Utils::display_ffmpeg_exception(res);

	// get video stream
	videoStream = get_video_stream();
	if (videoStream == -1)
		Utils::display_exception("Error opening your video using AVCodecParameters, probably doesnt have codecpar_type type AVMEDIA_TYPE_VIDEO");

	// open
	read_audio_video_codec();
}

void Player::clear()
{
	// close context info
	avformat_close_input(&pFormatCtx);
	avcodec_free_context(&pCodecCtx);

	// free buffers
	av_free(buffer);
	av_free(pFrameRGB);

	// Free the YUV frame
	av_free(pFrame);

	// Close the codecs
	avcodec_close(pCodecCtx);

	// Close the video file
	avformat_close_input(&pFormatCtx);

	delete Player::get_instance();
}

/*
Acquires video stream
*/
int Player::get_video_stream(void)
{
	int videoStream = -1;

	for (unsigned int i = 0; i<pFormatCtx->nb_streams; i++){
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) videoStream = i;
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) audioStream = i;
	}

	if (videoStream == -1)
		Utils::display_exception("Couldnt find stream");

	pCodecParameters = pFormatCtx->streams[videoStream]->codecpar;
	if(audioStream != -1) pCodecAudioParameters = pFormatCtx->streams[audioStream]->codecpar;

	return videoStream;
}

/*
Reads audio and video codec
*/
int Player::read_audio_video_codec(void) 
{
	pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
	pAudioCodec = avcodec_find_decoder(pCodecAudioParameters->codec_id);

	if (pCodec == NULL)
		Utils::display_exception("Video decoder not found");

	if (pAudioCodec == NULL) 
		Utils::display_exception("Audio decoder not found");

	pCodecCtx = avcodec_alloc_context3(pCodec);

	if(pCodecCtx == NULL)
		Utils::display_exception("Failed to allocate video context decoder");

	pCodecAudioCtx = avcodec_alloc_context3(pAudioCodec);

	if(pCodecAudioCtx == NULL)
		Utils::display_exception("Failed to allocate audio context decoder");

	int res = avcodec_parameters_to_context(pCodecCtx, pCodecParameters);

	if(res < 0)
		Utils::display_exception("Failed to transfer video parameters to context");

	res = avcodec_parameters_to_context(pCodecAudioCtx, pCodecAudioParameters);

	if (res < 0) 
		Utils::display_exception("Failed to transfer audio parameters to context");

	res = avcodec_open2(pCodecCtx, pCodec, NULL);

	if(res < 0)
		Utils::display_exception("Failed to open video codec");

	res = avcodec_open2(pCodecAudioCtx, pAudioCodec, NULL);

	if (res < 0)
		Utils::display_exception("Failed to open auvio codec");

	return 1;
}

/*
Alloc memory for the display
*/
int Player::malloc(void)
{
	Audio::get_instance()->malloc(pCodecAudioCtx);

	Audio::get_instance()->open();

	pFrame = av_frame_alloc();
	if (pFrame == NULL)
		Utils::display_exception("Couldnt allocate frame memory");

	pFrameRGB = av_frame_alloc();
	if (pFrameRGB == NULL)
		Utils::display_exception("Couldnt allocate rgb frame memory");

	int numBytes = av_image_get_buffer_size(VIDEO_FORMAT, pCodecCtx->width, pCodecCtx->height,1);

	buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	int res = av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, VIDEO_FORMAT, pCodecCtx->width, pCodecCtx->height, 1);
	if (res < 0)
		Utils::display_ffmpeg_exception(res);
	
	return 1;
}


int Player::getAudioPacket(AudioPacket* q, AVPacket* pkt, int block){

	AVPacketList* pktl;
    int ret;

    SDL_LockMutex(q->mutex);

    while (1)
    {
        pktl = q->first;
        if (pktl)
        {
            q->first = pktl->next;
            if (!q->first)
                q->last = NULL;

            q->nb_packets--;
            q->size -= pktl->pkt.size;

            *pkt = pktl->pkt;
            av_free(pktl);
            ret = 1;
            break;
        }
        else if (!block)
        {
            ret = 0;
            break;
        }
        else
        {
            SDL_CondWait(q->cond, q->mutex);
        }
    }

    SDL_UnlockMutex(q->mutex);

    return ret;
}

/*
Read frames and display
*/
int Player::display_video(void) {

	AVPacket packet;

	//video context
	sws_ctx = sws_getContext(pCodecCtx->width,
		pCodecCtx->height,
		pCodecCtx->pix_fmt,
		pCodecCtx->width,
		pCodecCtx->height,
		VIDEO_FORMAT,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL
		);
	SDL_Event evt;

	while (av_read_frame(pFormatCtx, &packet) >= 0) {

		if (packet.stream_index == audioStream) {
			Audio::get_instance()->put_audio_packet(&packet);
		}

		if (packet.stream_index == videoStream) 
		{

			int res = avcodec_send_packet(pCodecCtx, &packet);
			if (res < 0)
				Utils::display_ffmpeg_exception(res);

			res = avcodec_receive_frame(pCodecCtx, pFrame);
			
			SDL_UpdateYUVTexture(bmp, NULL, pFrame->data[0], pFrame->linesize[0],
				pFrame->data[1], pFrame->linesize[1],
				pFrame->data[2], pFrame->linesize[2]);
			SDL_RenderCopy(renderer, bmp, NULL, NULL);
			SDL_RenderPresent(renderer);
			SDL_UpdateWindowSurface(screen);
			SDL_Delay(1000/30);
		}

		SDL_PollEvent(&evt);

		av_packet_unref(&packet);
	}

	return 1;

}

/*
Create the display for the received video
*/
int Player::create_display(void) 
{
	screen = SDL_CreateWindow(window_name.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			pCodecCtx->width, pCodecCtx->height,
			SDL_WINDOW_OPENGL);
	
	if (!screen)
		Utils::display_exception("Couldn't show display window");

	renderer = SDL_CreateRenderer(screen, -1, 0);

	bmp = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STATIC, pCodecCtx->width, pCodecCtx->height);

	return 1;
}