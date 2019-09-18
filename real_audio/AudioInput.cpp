#include "AudioInput.h"
#include <assert.h>
#include <stdlib.h>



AudioInput::AudioInput(AudioDevicePtr audioDevice, unsigned int buff_size):audioDevice(audioDevice),buff_size(buff_size)
{
	captureQueue = new FrameQueue<AudioFramePtr>(6);
	swap_buff = (char*)malloc(buff_size);
}


AudioInput::~AudioInput()
{
	delete captureQueue;
	free(swap_buff);
}


void AudioInput::procCapture()
{
	while (true) {
		if (thread_stop)
			return;

		int pkt_offset = 0;

		pkt = av_packet_alloc();
		if (av_read_frame(pFmtCtx, pkt) != 0) {
			printf("read failed.\n");
			return;
		}


		int pkt_remain = pkt->size;
		bool ready = false;

		while (pkt_remain > 0) {
			int swap_remain = buff_size - swap_offset;

			if (pkt_remain >= swap_remain) {
				memcpy(swap_buff + swap_offset, pkt->data + pkt_offset, swap_remain);
				pkt_offset += swap_remain;
				swap_offset += swap_remain;
				pkt_remain -= swap_remain;
				ready = true;
			}
			else {
				memcpy(swap_buff + swap_offset, pkt->data + pkt_offset, pkt_remain);
				pkt_offset += pkt_remain;
				swap_offset += pkt_remain;
				pkt_remain = 0;
				ready = false;
			}

			if (ready) {
				char *data = (char*)malloc(buff_size);
				int len = buff_size;

				memcpy(data, swap_buff, buff_size);

				AudioFramePtr frameData = make_shared<AudioFrame>();
				frameData->setData(data, len);

				captureQueue->force_put(frameData);
				swap_offset = 0;
			}

		}

		av_packet_free(&pkt);
	}
}


bool AudioInput::open()
{
	pFmtCtx = avformat_alloc_context();
	AVInputFormat *pAudioInputFmt = NULL;

	avdevice_register_all();

	pAudioInputFmt = av_find_input_format("dshow");
	assert(pAudioInputFmt != NULL);

	AVDictionary *options = NULL;
	av_dict_set(&options, "audio_buffer_size", "30", 0);

	int ret = avformat_open_input(&pFmtCtx, audioDevice->getFFName(), pAudioInputFmt, &options);
	if (ret != 0) {
		printf("ret=%d\n", ret);
		return false;
	}

	thread_stop = false;
	threadCapture = new thread(&AudioInput::procCapture, this);

	return true;
}


void AudioInput::close()
{
	thread_stop = true;
	threadCapture->join();
}


void AudioInput::read(AudioFramePtr &audioFrame)
{
	captureQueue->get(audioFrame);
}
