#ifndef H_VIDEO_INPUT
#define H_VIDEO_INPUT

#include "Enviroment.h"
#include "VideoDevice.h"
#include "VideoFrame.h"
#include "FrameQueue.h"
#include <string>
#include <list>

using namespace std;

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



class VideoInput
{
public:
	VideoInput(VideoDevicePtr videoDevice, int width, int height);
	virtual ~VideoInput();

public:

	bool open();
	void close();
	void read(VideoFramePtr &videoFrame);

private:
	int width;
	int height;

	int videoindex;

	VideoDevicePtr videoDevice;
	FrameQueue<VideoFramePtr> *captureQueue;

	AVCodecContext *pCodecCtx;
	AVPacket *packet;
	AVFormatContext *pFormatCtx;
	struct SwsContext *img_convert_ctx;


	unsigned int buff_size;
	int swap_offset = 0;
	char *swap_buff;

	thread *threadCapture;
	bool thread_stop;
	void procCapture();

	string getResolution();
	void yuv_rotate_180(uint8_t* yuvbuf, int width, int height);
	void yuv_flip(uint8_t* yuvbuf, int width, int height);
};


#endif //H_VIDEO_INPUT
