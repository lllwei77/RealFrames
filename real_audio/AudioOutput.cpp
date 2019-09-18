#include "AudioOutput.h"
#include "Enviroment.h"



AudioOutput::AudioOutput(){
	use_default = true;
	playQueue = new FrameQueue<AudioFramePtr>(6);
}


AudioOutput::AudioOutput(AudioDevicePtr audioDevice):audioDevice(audioDevice) {
	use_default = false;
	playQueue = new FrameQueue<AudioFramePtr>(6);
}


void AudioOutput::procPlay()
{
	while (true) {
		AudioFramePtr frameData;
		playQueue->get(frameData);

		if (SDL_QueueAudio(dev, frameData->getData(), frameData->getSize()) < 0) {
			printf("error queuing audio: %s\n", SDL_GetError());
			SDL_Delay(1);
		}
	}
}


bool AudioOutput::open()
{
	SDL_AudioSpec want, have;

	SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
	/*
	want.freq = 48000;
	want.format = AUDIO_F32;
	want.channels = 2;
	want.samples = 4096;
	*/
	want.freq = 44100;
	want.format = AUDIO_S16SYS;
	want.channels = 2;
	want.silence = 0;
	want.samples = 1024;

	if (use_default) {
		if (SDL_OpenAudio(&want, NULL) < 0) {
			printf("can't open audio.\n");
			return false;
		}
		SDL_PauseAudio(0);
		dev = 1;
	}
	else {
		dev = SDL_OpenAudioDevice(audioDevice->getName(), 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

		if (dev == 0) {
			SDL_Log("Failed to open audio: %s", SDL_GetError());
		}
		else {
			if (have.format != want.format) { // we let this one thing change. 
				SDL_Log("We didn't get Float32 audio format.");
			}
			SDL_PauseAudioDevice(dev, 0); // start audio playing.
		}
	}

	thread_stop = false;
	threadPlay = new thread(&AudioOutput::procPlay, this);

	return true;
}


void AudioOutput::close()
{
	SDL_CloseAudioDevice(dev);
}


bool AudioOutput::write(AudioFramePtr &audioFrame)
{
	playQueue->force_put(audioFrame);
	return true;
}
