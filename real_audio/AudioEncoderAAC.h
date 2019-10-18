#ifndef AUDIO_ENCODER_AAC
#define AUDIO_ENCODER_AAC

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "AudioFrame.h"
#include "Enviroment.h"

#pragma warning(disable:4996)


class AudioEncoderAAC
{
public:
	AudioEncoderAAC();
	~AudioEncoderAAC();

	bool initialize();
	int  getInputBuffSize();
	bool encode(char *data_in, int size_in, char **data_out, int &size_out);
	bool encode(AudioFramePtr &inFrame, AudioFramePtr &outFrame);

private:
	AVFrame *frame;
	AVPacket *pkt;
	const AVCodec *codec;
	AVCodecContext *codecctx = NULL;

	SwrContext *swr;
};



#endif //AUDIO_ENCODER_AAC
