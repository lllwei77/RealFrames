#ifndef H_AUDIO_DEVICE_LIST
#define H_AUDIO_DEVICE_LIST

#include "AudioDevice.h"



class AudioDeviceHelper
{
public:
	static list<AudioDevicePtr> getInputDeviceList();
	static list<AudioDevicePtr> getOutputDeviceList();
};


#endif //H_AUDIO_DEVICE_LIST
