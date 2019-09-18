#ifndef H_VIDEO_FRAME
#define H_VIDEO_FRAME

#include "FrameData.h"
#include <memory>

#ifdef __cplusplus
extern "C"
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/parseutils.h>
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include <libavdevice/avdevice.h>

#ifdef __cplusplus
};
#endif



class VideoFrame : public FrameData
{
public:
	VideoFrame() {
		this->avFrame = NULL;
		this->avPacket = NULL;
		sws_scale_used = false;
	}

	VideoFrame(AVFrame *avFrame) {
		this->avFrame = avFrame;
	}

	virtual ~VideoFrame() {
		if (this->avFrame) {
			if(sws_scale_used)
			    av_free(this->avFrame->data[0]);   //for patching memory leak~~~
			av_frame_free(&this->avFrame);
		}

		if (this->avPacket) {
			av_packet_free(&this->avPacket);
		}
	}

public:
	void setAvFrame(AVFrame *avFrame) { this->avFrame = avFrame; }
	AVFrame *getAvFrame() { return this->avFrame; }

	void setAvPacket(AVPacket *avPacket) { this->avPacket = avPacket; }
	AVPacket *getAvPacket() { return this->avPacket; }

	void set_i_counter(int i_counter) { this->i_counter = i_counter; }
	int get_i_counter() { return i_counter; }

	void set_p_counter(int p_counter) { this->p_counter = p_counter; }
	int get_p_counter() { return p_counter; }

	void set_sws_scale_used() { this->sws_scale_used = true; };

private:
	AVFrame  *avFrame;
	AVPacket *avPacket;

	int i_counter;
	int p_counter;

	bool sws_scale_used;   //for patching memory leak~~~
};


typedef std::shared_ptr<VideoFrame> VideoFramePtr;


#endif //H_VIDEO_FRAME
