#ifndef H_AUDIO_OUTPUT
#define H_AUDIO_OUTPUT

#include "Enviroment.h"
#include "AudioDevice.h"
#include "AudioFrame.h"
#include "FrameQueue.h"
#include <string>
#include <list>

using namespace std;



class AudioOutput
{
public:
	AudioOutput();
	AudioOutput(AudioDevicePtr audioDevice);

	virtual ~AudioOutput() {}

public:

	bool open();
	void close();
	bool write(AudioFramePtr &audioFrame);

private:
	AudioDevicePtr audioDevice;
	SDL_AudioDeviceID dev;
	bool use_default;

	FrameQueue<AudioFramePtr> *playQueue;


	thread *threadPlay;
	bool thread_stop;
	void procPlay();
};


#endif //H_AUDIO_OUTPUT
