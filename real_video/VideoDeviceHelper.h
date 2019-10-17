#ifndef H_VIDEO_DEVICE_LIST
#define H_VIDEO_DEVICE_LIST

#include "VideoDevice.h"
#include <list>



class VideoDeviceHelper
{
public:
	static std::list<VideoDevicePtr> getDeviceList();
};


#endif //H_VIDEO_DEVICE_LIST
