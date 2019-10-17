#include "VideoDevice.h"
#include "Win_AVDevices.h"



VideoDeviceOption::VideoDeviceOption(int width, int height, int avg_time_perframe):
	width(width), height(height), avg_time_perframe(avg_time_perframe)
{
}


VideoDevice::VideoDevice(int device_id, const char *device_name)
{
	int device_name_len = (int)strlen(device_name);
	if (device_name_len > VIDEO_DEVICE_NAME_LEN)
		device_name_len = VIDEO_DEVICE_NAME_LEN;   //can not open this device anymore~~

	this->device_id = device_id;
	memset(this->device_name, 0, sizeof(this->device_name));
	memcpy(this->device_name, device_name, device_name_len);

	memset(device_ff_name, 0, sizeof(device_ff_name));
	sprintf(device_ff_name, "video=%s", this->device_name);

}


void VideoDevice::addOption(int width, int height, int avg_time_perframe)
{
	VideoDeviceOption deviceOption(width, height, avg_time_perframe);
	deviceOptionList.push_back(deviceOption);
}

