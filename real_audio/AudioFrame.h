#ifndef H_AUDIO_FRAME
#define H_AUDIO_FRAME

#include "FrameData.h"
#include <memory>


class AudioFrame : public FrameData
{
public:
	virtual ~AudioFrame(){}
};



typedef std::shared_ptr<AudioFrame> AudioFramePtr;


#endif //H_AUDIO_FRAME
