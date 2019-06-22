#ifndef H_AUDIO_INPUT
#define H_AUDIO_INPUT

#include "Enviroment.h"
#include "AudioDevice.h"
#include "AudioFrame.h"
#include "FrameQueue.h"
#include <string>
#include <list>

using namespace std;

#ifdef __cplusplus  
extern "C"
{
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/audio_fifo.h"

#ifdef __cplusplus
};
#endif



class AudioInput
{
public:
	AudioInput(AudioDevicePtr, unsigned int buff_size);
	virtual ~AudioInput();

public:

	bool open();
	void close();
	void read(AudioFramePtr &audioFrame);

private:
	AudioDevicePtr audioDevice;
	FrameQueue<AudioFramePtr> *captureQueue;

	AVFormatContext *pFmtCtx;
	AVPacket *pkt;

	unsigned int buff_size;
	int swap_offset = 0;
	char *swap_buff;

	thread *threadCapture;
	bool thread_stop;
	void procCapture();
};


#endif //H_AUDIO_INPUT
