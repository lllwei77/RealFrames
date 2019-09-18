#ifndef H_AUDIO_DEVICE
#define H_AUDIO_DEVICE


#include <string>
#include <list>
using namespace std;


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
	char device_name[256];
	char device_ff_name[256 + 6];  //"audio=xxx"

};


typedef shared_ptr<AudioDevice> AudioDevicePtr;



#endif //H_AUDIO_DEVICE
