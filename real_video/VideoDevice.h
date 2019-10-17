#ifndef H_VIDEO_DEVICE
#define H_VIDEO_DEVICE

#include <memory>
#include <list>

#define VIDEO_DEVICE_NAME_LEN 256


class VideoDeviceOption {
public:
	VideoDeviceOption(int width, int height, int avg_time_perframe);
	~VideoDeviceOption(){}

	int getWidth() { return width; }
	int getHeight() { return height; }
	int getAvgTimePerFrame() { return avg_time_perframe; }

private:
	int width;               //分辨率宽
	int height;              //分辨率高
	int avg_time_perframe;   //每帧的时间
};


class VideoDeviceHelper;

class VideoDevice
{
	friend class VideoDeviceHelper;
public:
	VideoDevice(int device_id, const char *device_name);
	virtual ~VideoDevice() {}

	int getID() { return device_id; }
	char* getName() { return device_name; }
	char* getFFName() { return device_ff_name; }

	std::list<VideoDeviceOption>& getOptionList() { return deviceOptionList; }

private:
	int  device_id;
	char device_name[256];
	char device_ff_name[256 + 6];  //"audio=xxx"

	void addOption(int width, int height, int avg_time_perframe);
	std::list<VideoDeviceOption> deviceOptionList;
};


typedef std::shared_ptr<VideoDevice> VideoDevicePtr;


#endif //H_VIDEO_DEVICE
