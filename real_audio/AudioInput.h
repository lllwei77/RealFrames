#ifndef H_AUDIO_INPUT
#define H_AUDIO_INPUT

#include "Enviroment.h"
#include "AudioDevice.h"
#include "AudioFrame.h"
#include "FrameQueue.h"
#include <string>
#include <list>
#include <thread>


#define BYTES_PER_FRAME_AAC   (1024*2*2)
#define BYTES_PER_FRAME_OPUS  (960*2*2)



class AudioInput
{
public:
	AudioInput(AudioDevicePtr, unsigned int bytes_per_frame);
	virtual ~AudioInput();

	bool open();
	void close();
	void read(AudioFramePtr &audioFrame);

private:
	AudioDevicePtr audioDevice;
	FrameQueue<AudioFramePtr> *captureQueue;

	AVFormatContext *pFmtCtx;
	AVPacket *pkt;

	unsigned int swap_buff_size;
	int swap_offset = 0;
	char *swap_buff;

	std::thread *threadCapture;
	bool thread_stop;
	void procCapture();
};


#endif //H_AUDIO_INPUT
