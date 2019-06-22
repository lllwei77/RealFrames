#ifndef H_AUDIO_DECODER
#define H_AUDIO_DECODER


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



class AudioDecoder
{
private:
	const AVCodec *codec;
	AVCodecContext *codecctx = NULL;
	AVCodecParserContext *parser = NULL;
	AVPacket *pkt;
	AVFrame *frame = NULL;

	char temp_data[1024 * 1024 * 2];

	SwrContext *swr;

public:
	AudioDecoder();
	~AudioDecoder();

	bool initialize();
	bool decode(char *data_in, int size_in, char **data_out, int &size_out);
	bool decode(AudioFramePtr &inFrame, AudioFramePtr &outFrame);
};


#endif //H_AUDIO_DECODER
