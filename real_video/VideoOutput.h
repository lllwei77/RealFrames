#ifndef H_VIDEO_OUTPUT
#define H_VIDEO_OUTPUT


#include "Enviroment.h"
#include "VideoDevice.h"
#include "VideoFrame.h"
#include "FrameQueue.h"
#include <string>
#include <list>

using namespace std;



class VideoOutput
{
public:
	VideoOutput(int width, int height);
	VideoOutput(void* winHdl, int width, int height);
	virtual ~VideoOutput() {}

public:

	bool open();
	void close();
	bool write(VideoFramePtr &videoFrame);

private:
	FrameQueue<VideoFramePtr> *playQueue;

	thread *threadPlay;
	bool thread_stop;
	void procPlay();

	void* winHdl;
	int screen_w = 0, screen_h = 0;
	SDL_Window *screen;
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;
	SDL_Rect sdlRect;
};



#endif //H_VIDEO_OUTPUT
