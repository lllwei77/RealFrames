#include "VideoDevice.h"
#include "Enviroment.h"
#include "Win_AVDevices.h"



char *_dup_wchar_to_utf8(const wchar_t *w)
{
	char *s = NULL;
	int l = WideCharToMultiByte(CP_UTF8, 0, w, -1, 0, 0, 0, 0);
	s = (char *)malloc(l);
	if (s)
		WideCharToMultiByte(CP_UTF8, 0, w, -1, s, l, 0, 0);
	return s;
}


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


list<shared_ptr<VideoDevice>> getVideoDeviceList()
{
	list<shared_ptr<VideoDevice>> deviceList;

	HRESULT hrrst;
	GUID guid = CLSID_VideoInputDeviceCategory;
	std::vector<DeviceInfo> videoDeviceVec;
	hrrst = DsGetAudioVideoInputDevices(videoDeviceVec, guid);

	if (SUCCEEDED(hrrst)) {
		for (int i = 0; i < videoDeviceVec.size(); i++) {
			DeviceInfo deviceInfo = videoDeviceVec[i];
			shared_ptr<VideoDevice> videoDevice = std::make_shared<VideoDevice>(i, _dup_wchar_to_utf8(deviceInfo.FriendlyName));

			for (int i = 0; i < deviceInfo.paramCount; i++) {
				DeviceParam &parm = deviceInfo.Params[i];
				videoDevice->addOption(parm.width, parm.height, parm.avgTimePerFrame);
			}
			deviceList.push_back(videoDevice);
		}
	}

	return deviceList;
}
