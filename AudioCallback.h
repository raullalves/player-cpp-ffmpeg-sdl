#pragma once
class AudioCallback
{
public:
	static void set_audio_instance(Audio*);
	static void audio_callback(void*, Uint8*, int);

private:
	AudioCallback() {}

	static Audio* audio_instance;
};