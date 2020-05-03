#pragma once
typedef struct _AudioPacket
{
	AVPacketList* first, * last;
	int nb_packets, size;
	SDL_mutex* mutex;
	SDL_cond* cond;
} AudioPacket;