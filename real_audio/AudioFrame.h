#ifndef H_AUDIO_FRAME
#define H_AUDIO_FRAME


#include "FrameData.h"
#include <memory>


typedef std::shared_ptr<FrameData> AudioFramePtr;

AudioFramePtr makeAudioFrame();



#endif //H_AUDIO_FRAME
