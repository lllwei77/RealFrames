#include "VideoFrame.h"


VideoFramePtr makeVideoFrame()
{
	VideoFramePtr videoFrame = std::make_shared<VideoFrame>();
	return videoFrame;
}
