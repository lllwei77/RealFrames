#ifndef H_AUDIO_DEVICE_LIST
#define H_AUDIO_DEVICE_LIST

#include "AudioDevice.h"
#include <list>


class AudioDeviceHelper
{
public:
	static std::list<AudioDevicePtr> getInputDeviceList();
	static std::list<AudioDevicePtr> getOutputDeviceList();
};


#endif //H_AUDIO_DEVICE_LIST
