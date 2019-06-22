#include "AudioDevice.h"
#include "Enviroment.h"



AudioDevice::AudioDevice(int device_id, char *device_name)
{
	this->device_id = device_id;
	memset(this->device_name, 0, sizeof(this->device_name));
	memcpy(this->device_name, device_name, strlen(device_name));

	memset(device_ff_name, 0, sizeof(device_ff_name));
	sprintf(device_ff_name, "audio=%s", device_name);

}


list<shared_ptr<AudioDevice>> getAudioDeviceList(bool is_capture)
{
	list<shared_ptr<AudioDevice>> deviceList;
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
