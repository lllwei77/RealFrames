#include <stdio.h>
#include <stdlib.h>

#include "Enviroment.h"
#include "AudioDevice.h"
#include "AudioDeviceHelper.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "AudioEncoderAAC.h"
#include "AudioDecoderAAC.h"
#include "AudioEncoderOpus.h"
#include "AudioDecoderOpus.h"

#include "AudioFrame.h"

#include "VideoDevice.h"
#include "VideoFrame.h"
#include "VideoInput.h"
#include "VideoOutput.h"
#include "VideoDeviceHelper.h"
#include "VideoEncoderH265.h"
#include "VideoDecoderH265.h"
#include "VideoEncoderVP9.h"
#include "VideoDecoderVP9.h"



void testAudioOpus()
{
	list<AudioDevicePtr> deviceList = AudioDeviceHelper::getInputDeviceList();
	for (AudioDevicePtr audioDevice : deviceList) {
		printf("input device[%d] : %s\n", audioDevice->getID(), audioDevice->getName());
	}
	AudioDevicePtr audioDevice = deviceList.front();
	AudioInput *audioInput = new AudioInput(audioDevice, BYTES_PER_FRAME_OPUS);


	list<AudioDevicePtr> deviceList2 = AudioDeviceHelper::getOutputDeviceList();
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

	AudioEncoderOpus *audioEncoderOpus = new AudioEncoderOpus();
	ret = audioEncoderOpus->initialize();
	if (!ret) {
		printf("init audioEncoder failed.\n");
		exit(-1);
	}

	AudioDecoderOpus *audioDecoderOpus = new AudioDecoderOpus();
	ret = audioDecoderOpus->initialize();
	if (!ret) {
		printf("init audioDecoder failed.\n");
		exit(-1);
	}


	for (int i = 0; i < 100000; i++) {

		AudioFramePtr encInFrame, encOutFrame, decInFrame, decOutFrame;
		audioInput->read(encInFrame);

		ret = audioEncoderOpus->encode(encInFrame, encOutFrame);
		if (!ret) {
			printf("encode failed.\n");
			//exit(-1);
		}
		else {
			decInFrame = encOutFrame;
			ret = audioDecoderOpus->decode(decInFrame, decOutFrame);
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


void testAudioAAC()
{
	list<AudioDevicePtr> deviceList = AudioDeviceHelper::getInputDeviceList();
	for (AudioDevicePtr audioDevice : deviceList) {
		printf("input device[%d] : %s\n", audioDevice->getID(), audioDevice->getName());
	}
	AudioDevicePtr audioDevice = deviceList.front();
	AudioInput *audioInput = new AudioInput(audioDevice, BYTES_PER_FRAME_AAC);


	list<AudioDevicePtr> deviceList2 = AudioDeviceHelper::getOutputDeviceList();
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

	AudioEncoderAAC *audioEncoderAAC = new AudioEncoderAAC();
	ret = audioEncoderAAC->initialize();
	if (!ret) {
		printf("init audioEncoder failed.\n");
		exit(-1);
	}

	AudioDecoderAAC *audioDecoderAAC = new AudioDecoderAAC();
	ret = audioDecoderAAC->initialize();
	if (!ret) {
		printf("init audioDecoder failed.\n");
		exit(-1);
	}


	for (int i = 0; i < 100000; i++) {

		AudioFramePtr encInFrame, encOutFrame, decInFrame, decOutFrame;
		audioInput->read(encInFrame);

		ret = audioEncoderAAC->encode(encInFrame, encOutFrame);
		if (!ret) {
			printf("encode failed.\n");
			//exit(-1);
		}

		else {
			decInFrame = encOutFrame;
			ret = audioDecoderAAC->decode(decInFrame, decOutFrame);
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


void testVideoH265()
{
	list<VideoDevicePtr> videoDiviceList = VideoDeviceHelper::getDeviceList();
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

		VideoEncoderH265 *encoder = new VideoEncoderH265();
		encoder->init(1280, 720);
		VideoDecoderH265 *decoder = new VideoDecoderH265();
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


void testVideoVP9()
{
	list<VideoDevicePtr> videoDiviceList = VideoDeviceHelper::getDeviceList();
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

		VideoEncoderVP9 *encoder = new VideoEncoderVP9();
		encoder->init(1280, 720);
		VideoDecoderVP9 *decoder = new VideoDecoderVP9();
		decoder->init();

		while (true) {
			VideoFramePtr videoFrame;
			videoInput->read(videoFrame);

			VideoFramePtr encodedFrame;
			VideoFramePtr decodedFrame;

			ret = encoder->encode(videoFrame, encodedFrame);
			if (!ret)
				//exit(-1);
				continue;

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

	//testAudioAAC();
	//testAudioOpus();
	
	testVideoH265();
	//testVideoVP9();

	Enviroment::SDLDestory();

	exit(0);
}
