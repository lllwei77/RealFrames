#include <stdio.h>
#include <stdlib.h>

#include "Enviroment.h"
#include "AudioDevice.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "AudioEncoder.h"
#include "AudioDecoder.h"
#include "AudioFrame.h"

#include "VideoDevice.h"
#include "VideoFrame.h"
#include "VideoInput.h"
#include "VideoOutput.h"
#include "VideoEncoder.h"
#include "VideoDecoder.h"




void testAudio()
{
	list<AudioDevicePtr> deviceList = getAudioDeviceList(true);
	for (AudioDevicePtr audioDevice : deviceList) {
		printf("input device[%d] : %s\n", audioDevice->getID(), audioDevice->getName());
	}
	AudioDevicePtr audioDevice = deviceList.front();
	AudioInput *audioInput = new AudioInput(audioDevice, 4096);


	list<AudioDevicePtr> deviceList2 = getAudioDeviceList(false);
	for (AudioDevicePtr audioDevice : deviceList2) {
		printf("output device[%d] : %s\n", audioDevice->getID(), audioDevice->getName());
	}
	AudioDevicePtr outputDevice = deviceList2.front();

	AudioOutput *audioOutput = new AudioOutput();
	//AudioOutput *audioOutput = new AudioOutput(outputDevice);

	bool ret = audioInput->open();
	if (!ret) {
		printf("input audio open failed.\n");
		exit(-1);
	}

	ret = audioOutput->open();
	if (!ret) {
		printf("output audio open failed.\n");
		exit(-1);
	}

	AudioEncoder *audioEncoder = new AudioEncoder();
	ret = audioEncoder->initialize();
	if (!ret) {
		printf("init audioEncoder failed.\n");
		exit(-1);
	}

	AudioDecoder *audioDecoder = new AudioDecoder();
	ret = audioDecoder->initialize();
	if (!ret) {
		printf("init audioDecoder failed.\n");
		exit(-1);
	}


	for (int i = 0; i < 100000; i++) {

		AudioFramePtr encInFrame, encOutFrame, decInFrame, decOutFrame;
		audioInput->read(encInFrame);

		ret = audioEncoder->encode(encInFrame, encOutFrame);
		if (!ret) {
			printf("encode failed.\n");
			//exit(-1);
		}
		else {
			decInFrame = encOutFrame;
			ret = audioDecoder->decode(decInFrame, decOutFrame);
			if (!ret) {
				printf("decode failed.\n");
				//exit(-1);
			}
			else {
				audioOutput->write(decOutFrame);
			}
		}

	}

}


void testVideo()
{
	list<VideoDevicePtr> videoDiviceList = getVideoDeviceList();
	for (VideoDevicePtr videoDevice : videoDiviceList) {
		printf("%s\n", videoDevice->getFFName());
		list<VideoDeviceOption> &deviceOptionList = videoDevice->getOptionList();
		for (VideoDeviceOption deviceOption : deviceOptionList) {
			printf("%d x %d, %d\n", deviceOption.getWidth(), deviceOption.getHeight(), deviceOption.getAvgTimePerFrame());
		}

		VideoInput *videoInput = new VideoInput(videoDevice, 1280, 720);
		bool ret = videoInput->open();
		if (!ret)
			exit(-1);

		VideoOutput *videoOutput = new VideoOutput(1280, 720);
		ret = videoOutput->open();
		if (!ret)
			exit(-1);

		VideoEncoder *encoder = new VideoEncoder();
		encoder->init(1280, 720);
		VideoDecoder *decoder = new VideoDecoder();
		decoder->init();

		while (true) {
			VideoFramePtr videoFrame;
			videoInput->read(videoFrame);

			VideoFramePtr encodedFrame;
			VideoFramePtr decodedFrame;

			ret = encoder->encode(videoFrame, encodedFrame);
			if (!ret)
				exit(-1);

			ret = decoder->decode(encodedFrame, decodedFrame);
			if (!ret)
				exit(-1);

			videoOutput->write(decodedFrame);
		}
	}
}


int main(int argc, char *argv[])
{
	printf("hello realstream!\n");

	bool ret = Enviroment::SDLInit();
	if (!ret) {
		printf("sdl init failed.\n");
		exit(-1);
	}

	//testAudio();
	testVideo();

	Enviroment::SDLDestory();

	exit(0);
}
