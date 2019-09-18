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

