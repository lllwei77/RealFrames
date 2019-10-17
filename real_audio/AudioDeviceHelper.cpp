#include "AudioDeviceHelper.h"
#include "Enviroment.h"
#include <memory>



static std::list<AudioDevicePtr> getDeviceList(bool is_capture)
{
	std::list<std::shared_ptr<AudioDevice>> deviceList;
	int iscapture = is_capture ? 1 : 0;

	int i, count = SDL_GetNumAudioDevices(iscapture);
	for (i = 0; i < count; ++i) {
		char *device_name = (char*)SDL_GetAudioDeviceName(i, iscapture);
		if (device_name) {
			AudioDevicePtr audioDevice(new AudioDevice(i, device_name));
			deviceList.push_back(audioDevice);
		}
	}

	return deviceList;
}


std::list<AudioDevicePtr> AudioDeviceHelper::getInputDeviceList()
{
	return getDeviceList(true);
}


std::list<AudioDevicePtr> AudioDeviceHelper::getOutputDeviceList()
{
	return getDeviceList(false);
}

