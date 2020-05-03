#pragma once
class Audio
{
public:
	static Audio* get_instance();

	struct SwrContext* swrCtx = NULL;
	AVFrame wanted_frame;

	AudioPacket audioq;

	void open();
	void malloc(AVCodecContext*);
	void init_audio_packet(AudioPacket*);
	int audio_decode_frame(AVCodecContext*, uint8_t*, int);
	int put_audio_packet(AVPacket*);

private:
	Audio() {}
	static Audio* instance;

	SDL_AudioSpec wantedSpec = { 0 }, audioSpec = { 0 };
};