#ifndef H_SDL_ENVIROMENT
#define H_SDL_ENVIROMENT

#include <SDL/SDL.h>
#undef main


#ifdef __cplusplus  
extern "C"
{
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavutil/frame.h"
#include "libavutil/samplefmt.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"  
#include "libavdevice/avdevice.h"

#ifdef __cplusplus
};
#endif



class Enviroment
{
public:
	virtual ~Enviroment() {};

public:
	static bool SDLInit();
	static void SDLDestory();

};


#endif //H_SDL_ENVIROMENT
