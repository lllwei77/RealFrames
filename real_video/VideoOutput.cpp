#include "VideoOutput.h"
#include "Enviroment.h"



VideoOutput::VideoOutput(int width, int height) {
	this->winHdl = nullptr;
	playQueue = new FrameQueue<VideoFramePtr>(6);
	screen_w = width;
	screen_h = height;
}


VideoOutput::VideoOutput(void* winHdl, int width, int height)
{
	this->winHdl = winHdl;
	screen_w = width;
	screen_h = height;
}


void VideoOutput::procPlay()
{
	while (true) {
		VideoFramePtr videoFrame;
		playQueue->get(videoFrame);

		AVFrame *pFrame = videoFrame->getAvFrame();

		SDL_UpdateYUVTexture(sdlTexture, &sdlRect,
			pFrame->data[0], pFrame->linesize[0],
			pFrame->data[1], pFrame->linesize[1],
			pFrame->data[2], pFrame->linesize[2]);

		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
		SDL_RenderPresent(sdlRenderer);
	}
}


bool VideoOutput::open()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return false;
	}

	if (winHdl > 0) {
		screen = SDL_CreateWindowFrom((const void *)winHdl);
	}
	else {
		screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	   	screen_w, screen_h,
	    SDL_WINDOW_OPENGL);
	}

	if (!screen) {
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return false;
	}

	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, screen_w, screen_h);

	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = screen_w;
	sdlRect.h = screen_h;

	thread_stop = false;
	threadPlay = new thread(&VideoOutput::procPlay, this);

	return true;
}


void VideoOutput::close()
{
}


bool VideoOutput::write(VideoFramePtr &videoFrame)
{
	playQueue->force_put(videoFrame);
	return true;
}

