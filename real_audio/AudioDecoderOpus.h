#ifndef H_AUDIO_DECODER_OPUS
#define H_AUDIO_DECODER_OPUS


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "AudioFrame.h"
#include "Enviroment.h"


class AudioDecoderOpus
{
public:
	AudioDecoderOpus();
	~AudioDecoderOpus();

	bool initialize();
	bool decode(char *data_in, int size_in, char **data_out, int &size_out);
	bool decode(AudioFramePtr &inFrame, AudioFramePtr &outFrame);

private:
	const AVCodec *codec;
	AVCodecContext *codecctx = NULL;
	AVCodecParserContext *parser = NULL;
	AVPacket *pkt;
	AVFrame *frame = NULL;

	char temp_data[1024 * 1024 * 2];

	SwrContext *swr;
};


#endif //H_AUDIO_DECODER_OPUS
