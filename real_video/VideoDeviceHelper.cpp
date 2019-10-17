#include "VideoDeviceHelper.h"
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


std::list<VideoDevicePtr> VideoDeviceHelper::getDeviceList()
{
	std::list<std::shared_ptr<VideoDevice>> deviceList;

	HRESULT hrrst;
	GUID guid = CLSID_VideoInputDeviceCategory;
	std::vector<DeviceInfo> videoDeviceVec;
	hrrst = DsGetAudioVideoInputDevices(videoDeviceVec, guid);

	if (SUCCEEDED(hrrst)) {
		for (int i = 0; i < videoDeviceVec.size(); i++) {
			DeviceInfo deviceInfo = videoDeviceVec[i];
			std::shared_ptr<VideoDevice> videoDevice = std::make_shared<VideoDevice>(i, _dup_wchar_to_utf8(deviceInfo.FriendlyName));

			for (int i = 0; i < deviceInfo.paramCount; i++) {
				DeviceParam &parm = deviceInfo.Params[i];
				videoDevice->addOption(parm.width, parm.height, parm.avgTimePerFrame);
			}

			deviceList.push_back(videoDevice);
		}
	}

	return deviceList;
}
