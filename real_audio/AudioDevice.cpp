#include "AudioDevice.h"
#include "Enviroment.h"



AudioDevice::AudioDevice(int device_id, char *device_name)
{
	int device_name_len = (int)strlen(device_name);
	if (device_name_len > AUDIO_DEVICE_NAME_LEN)
		device_name_len = AUDIO_DEVICE_NAME_LEN;   //can not open this device anymore~~

	this->device_id = device_id;
	memset(this->device_name, 0, sizeof(this->device_name));
	memcpy(this->device_name, device_name, device_name_len);

	memset(device_ff_name, 0, sizeof(device_ff_name));
	sprintf(device_ff_name, "audio=%s", this->device_name);
}

