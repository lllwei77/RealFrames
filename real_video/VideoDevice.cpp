#include "VideoDevice.h"
#include "Enviroment.h"
#include "Win_AVDevices.h"



VideoDeviceOption::VideoDeviceOption(int width, int height, int avg_time_perframe):
	width(width), height(height), avg_time_perframe(avg_time_perframe)
{

}


VideoDevice::VideoDevice(int device_id, const char *device_name)
{
	this->device_id = device_id;
	memset(this->device_name, 0, sizeof(this->device_name));
	memcpy(this->device_name, device_name, strlen(device_name));

	memset(device_ff_name, 0, sizeof(device_ff_name));
	sprintf(device_ff_name, "video=%s", device_name);

}


void VideoDevice::addOption(int width, int height, int avg_time_perframe)
{
	VideoDeviceOption deviceOption(width, height, avg_time_perframe);
	deviceOptionList.push_back(deviceOption);
}


