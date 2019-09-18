#ifndef H_AUDIO_ENCODER_OPUS
#define H_AUDIO_ENCODER_OPUS

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "AudioFrame.h"


#ifdef __cplusplus  
extern "C"
{
#endif 

#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
#include "libavformat/avformat.h"  
#include "libswscale/swscale.h"  
#include "libavdevice/avdevice.h"

#ifdef __cplusplus  
};
#endif

#pragma warning(disable:4996)


class AudioEncoderOpus
{
private:
	AVFrame *frame;
	AVPacket *pkt;
	const AVCodec *codec;
	AVCodecContext *codecctx = NULL;

	SwrContext *swr;

public:
	AudioEncoderOpus();
	~AudioEncoderOpus();

	bool initialize();
	int  getInputBuffSize();
	bool encode(char *data_in, int size_in, char **data_out, int &size_out);
	bool encode(AudioFramePtr &inFrame, AudioFramePtr &outFrame);

};



#endif //H_AUDIO_ENCODER_OPUS
