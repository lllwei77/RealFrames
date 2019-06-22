#ifndef H_VIDEO_DECODER
#define H_VIDEO_DECODER

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#ifdef __cplusplus
extern "C"
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/parseutils.h>

#ifdef __cplusplus
};
#endif


#include "VideoFrame.h"



class VideoDecoder
{
private:
	AVFormatContext* pFormatCtx;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;
	AVCodecParserContext *parser = NULL;

public:
	VideoDecoder();
	~VideoDecoder();
	bool init();

	bool decode(VideoFramePtr &inFrame, VideoFramePtr &outFrame);

private:
	short ins_i_counter = 0;
	short ins_p_counter = 0;

	AVFrame* decode(AVPacket *pPacket, int i_counter, int p_counter);
};


#endif /*H_VIDEO_DECODER*/
