#ifndef H_VIDEO_DEVICE_LIST
#define H_VIDEO_DEVICE_LIST

#include "VideoDevice.h"



class VideoDeviceHelper
{
public:
	static list<VideoDevicePtr> getDeviceList();
};


#endif //H_VIDEO_DEVICE_LIST
