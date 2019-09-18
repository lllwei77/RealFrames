#ifndef H_VIDEO_ENCODER_VP9
#define H_VIDEO_ENCODER_VP9

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



class VideoEncoderVP9
{
private:
	AVFormatContext* pFormatCtx;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;
	//AVFrame *pFrame;

public:
	VideoEncoderVP9();
	~VideoEncoderVP9();
	bool init(int width, int height);

	bool encode(VideoFramePtr &inFrame, VideoFramePtr &outFrame);

private:
	int set_I_interval = 30;
	short ins_i_counter = 0;
	short ins_p_counter = 0;

	AVPacket* encode(AVFrame *pFrame, int &i_counter, int &p_counter);
};


#endif /*H_VIDEO_ENCODER*/
