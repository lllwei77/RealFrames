#ifndef H_AUDIO_DEVICE
#define H_AUDIO_DEVICE

#include <memory>
#define AUDIO_DEVICE_NAME_LEN 256



class AudioDevice
{
public:
	AudioDevice(int device_id, char *device_name);
	virtual ~AudioDevice() {}

	int getID() { return device_id; }
	char* getName() { return device_name; }
	char* getFFName() { return device_ff_name; }

private:
	int  device_id;
	char device_name[AUDIO_DEVICE_NAME_LEN];
	char device_ff_name[AUDIO_DEVICE_NAME_LEN + 6];  //"audio=xxx"

};


typedef std::shared_ptr<AudioDevice> AudioDevicePtr;



#endif //H_AUDIO_DEVICE
