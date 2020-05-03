#include "stdafx.h"

Audio* Audio::instance = 0;

Audio* Audio::get_instance()
{
    if (instance == 0)
        instance = new Audio();
    return instance;
}

void Audio::open()
{
    SDLWrapper::open_audio(&wantedSpec, &audioSpec);

    wanted_frame.format = AV_SAMPLE_FMT_S16;
    wanted_frame.sample_rate = audioSpec.freq;
    wanted_frame.channel_layout = av_get_default_channel_layout(audioSpec.channels);
    wanted_frame.channels = audioSpec.channels;

    init_audio_packet(&audioq);
    SDL_PauseAudio(0);
}

void Audio::init_audio_packet(AudioPacket* q)
{
    q->last = NULL;
    q->first = NULL;
    q->mutex = SDL_CreateMutex();
    q->cond = SDL_CreateCond();
}

void Audio::malloc(AVCodecContext* pCodecAudioCtx)
{
    AudioCallback::set_audio_instance(this);

    swrCtx = swr_alloc();
    if (swrCtx == NULL)
        Utils::display_exception("Failed to load audio");

    av_opt_set_channel_layout(swrCtx, "in_channel_layout", pCodecAudioCtx->channel_layout, 0);
    av_opt_set_channel_layout(swrCtx, "out_channel_layout", pCodecAudioCtx->channel_layout, 0);
    av_opt_set_int(swrCtx, "in_sample_rate", pCodecAudioCtx->sample_rate, 0);
    av_opt_set_int(swrCtx, "out_sample_rate", pCodecAudioCtx->sample_rate, 0);
    av_opt_set_sample_fmt(swrCtx, "in_sample_fmt", pCodecAudioCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(swrCtx, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);

    int res = swr_init(swrCtx);

    if (res != 0)
        Utils::display_exception("Failed to initialize audio");

    memset(&wantedSpec, 0, sizeof(wantedSpec));

    wantedSpec.channels = pCodecAudioCtx->channels;
    wantedSpec.freq = pCodecAudioCtx->sample_rate;
    wantedSpec.format = AUDIO_S16SYS;
    wantedSpec.silence = 0;
    wantedSpec.samples = SDL_AUDIO_BUFFER_SIZE;
    wantedSpec.userdata = pCodecAudioCtx;
    wantedSpec.callback = AudioCallback::audio_callback;
}

int Audio::audio_decode_frame(AVCodecContext* aCodecCtx, uint8_t* audio_buf, int buf_size) {

    static AVPacket pkt;
    static uint8_t* audio_pkt_data = NULL;
    static int audio_pkt_size = 0;
    static AVFrame frame;

    int len1;
    int data_size = 0;

    SwrContext* swr_ctx = NULL;

    while (1)
    {
        while (audio_pkt_size > 0)
        {
            int got_frame = 0;

            avcodec_send_packet(aCodecCtx, &pkt);
            avcodec_receive_frame(aCodecCtx, &frame);

            len1 = frame.pkt_size;
            if (len1 < 0)
            {
                audio_pkt_size = 0;
                break;
            }

            audio_pkt_data += len1;
            audio_pkt_size -= len1;

            data_size = 0;

            if (got_frame)
            {
                int linesize = 1;
                data_size = av_samples_get_buffer_size(&linesize, aCodecCtx->channels, frame.nb_samples, aCodecCtx->sample_fmt, 1);
                assert(data_size <= buf_size);
                memcpy(audio_buf, frame.data[0], data_size);
            }

            if (frame.channels > 0 && frame.channel_layout == 0)
                frame.channel_layout = av_get_default_channel_layout(frame.channels);
            else if (frame.channels == 0 && frame.channel_layout > 0)
                frame.channels = av_get_channel_layout_nb_channels(frame.channel_layout);

            if (swr_ctx)
            {
                swr_free(&swr_ctx);
                swr_ctx = NULL;
            }

            swr_ctx = swr_alloc_set_opts(NULL, wanted_frame.channel_layout, (AVSampleFormat)wanted_frame.format, wanted_frame.sample_rate,
                frame.channel_layout, (AVSampleFormat)frame.format, frame.sample_rate, 0, NULL);

            if (!swr_ctx || swr_init(swr_ctx) < 0)
                Utils::display_exception("swr_init failed");

            int dst_nb_samples = (int)av_rescale_rnd(swr_get_delay(swr_ctx, frame.sample_rate) + frame.nb_samples,
                wanted_frame.sample_rate, wanted_frame.format, AV_ROUND_INF);

            int len2 = swr_convert(swr_ctx, &audio_buf, dst_nb_samples,
                (const uint8_t**)frame.data, frame.nb_samples);
            if (len2 < 0)
                Utils::display_exception("swr_convert failed");

            av_packet_unref(&pkt);

            if (swr_ctx)
            {
                swr_free(&swr_ctx);
                swr_ctx = NULL;
            }

            return wanted_frame.channels * len2 * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
        }

        if (Player::get_instance()->getAudioPacket(&audioq, &pkt, 1) < 0)
            return -1;

        audio_pkt_data = pkt.data;
        audio_pkt_size = pkt.size;
    }
}

int Audio::put_audio_packet(AVPacket* packet)
{
    AVPacketList* pktl;
    AVPacket* newPkt;
    newPkt = (AVPacket*)av_mallocz_array(1, sizeof(AVPacket));
    if (av_packet_ref(newPkt, packet) < 0)
        return -1;

    pktl = (AVPacketList*)av_malloc(sizeof(AVPacketList));
    if (!pktl)
        return -1;

    pktl->pkt = *newPkt;
    pktl->next = NULL;

    SDL_LockMutex(audioq.mutex);

    if (!audioq.last)
        audioq.first = pktl;
    else
        audioq.last->next = pktl;

    audioq.last = pktl;

    audioq.nb_packets++;
    audioq.size += newPkt->size;

    SDL_CondSignal(audioq.cond);
    SDL_UnlockMutex(audioq.mutex);

    return 0;
}