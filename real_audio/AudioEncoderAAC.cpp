#include "AudioEncoderAAC.h"



/**
*  Add ADTS header at the beginning of each and every AAC packet.
*  This is needed as MediaCodec encoder generates a packet of raw
*  AAC data.
*
*  Note the packetLen must count in the ADTS header itself !!! .
*注意，这里的packetLen参数为raw aac Packet Len + 7; 7 bytes adts header
**/

void addADTStoPacket(char* packet, int packetLen) {
	int profile = 2;  //AAC LC，MediaCodecInfo.CodecProfileLevel.AACObjectLC;
	int freqIdx = 4;  //32K, 见后面注释avpriv_mpeg4audio_sample_rates中32000对应的数组下标，来自ffmpeg源码
	int chanCfg = 2;  //见后面注释channel_configuration，Stero双声道立体声

					  /*int avpriv_mpeg4audio_sample_rates[] = {
					  96000, 88200, 64000, 48000, 44100, 32000,
					  24000, 22050, 16000, 12000, 11025, 8000, 7350
					  };
					  channel_configuration: 表示声道数chanCfg
					  0: Defined in AOT Specifc Config
					  1: 1 channel: front-center
					  2: 2 channels: front-left, front-right
					  3: 3 channels: front-center, front-left, front-right
					  4: 4 channels: front-center, front-left, front-right, back-center
					  5: 5 channels: front-center, front-left, front-right, back-left, back-right
					  6: 6 channels: front-center, front-left, front-right, back-left, back-right, LFE-channel
					  7: 8 channels: front-center, front-left, front-right, side-left, side-right, back-left, back-right, LFE-channel
					  8-15: Reserved
					  */

					  // fill in ADTS data
	packet[0] = 0xFF;
	packet[1] = 0xF9;
	packet[2] = (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
	packet[3] = (((chanCfg & 3) << 6) + (packetLen >> 11));
	packet[4] = ((packetLen & 0x7FF) >> 3);
	packet[5] = (((packetLen & 7) << 5) + 0x1F);
	packet[6] = 0xFC;
}


AudioEncoderAAC::AudioEncoderAAC()
{
	frame = av_frame_alloc();
	pkt = av_packet_alloc();

	swr = swr_alloc();
	av_opt_set_int(swr, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
	av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
	av_opt_set_int(swr, "in_sample_rate", 44100, 0);
	av_opt_set_int(swr, "out_sample_rate", 44100, 0);
	av_opt_set_sample_fmt(swr, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
	av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_FLTP, 0);
	swr_init(swr);
}


AudioEncoderAAC::~AudioEncoderAAC()
{
	av_frame_free(&frame);
	av_packet_free(&pkt);
	if (codecctx) avcodec_free_context(&codecctx);

	swr_close(swr);
	swr_free(&swr);
}


bool AudioEncoderAAC::initialize()
{
	codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		return false;
	}

	codecctx = avcodec_alloc_context3(codec);
	if (!codecctx) {
		fprintf(stderr, "Could not allocate audio codec context\n");
		return false;
	}

	codecctx->bit_rate = 64000;
	//codecctx->sample_fmt = AV_SAMPLE_FMT_S16;  //s16 pcm
	codecctx->sample_fmt = AV_SAMPLE_FMT_FLTP;
	codecctx->sample_rate = 44100;
	codecctx->channel_layout = AV_CH_LAYOUT_STEREO;
	codecctx->channels = 2;  //av_get_channel_layout_nb_channels(codecctx->channel_layout)

	if (avcodec_open2(codecctx, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		return false;
	}

	frame->nb_samples = codecctx->frame_size;
	frame->format = codecctx->sample_fmt;
	frame->channel_layout = codecctx->channel_layout;

	/* allocate the data buffers */
	int ret = av_frame_get_buffer(frame, 0);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate audio data buffers\n");
		return false;
	}

	return true;
}


int AudioEncoderAAC::getInputBuffSize()
{
	return codecctx->frame_size * 4;
}


bool AudioEncoderAAC::encode(char *data_in, int size_in, char **data_out, int &size_out)
{
	int ret;

	ret = av_frame_make_writable(frame);
	if (ret < 0)
		return false;

	uint8_t *ins[2];
	ins[0] = (uint8_t*)data_in;
	ins[1] = (uint8_t*)data_in + 2048;

	uint8_t *outs[2];
	outs[0] = (uint8_t*)frame->data[0];
	outs[1] = (uint8_t*)frame->data[1];

	int count = swr_convert(swr, outs, frame->nb_samples, (const uint8_t **)ins, frame->nb_samples);

	int malloc_size = size_in * 2;
	*data_out = (char*)malloc(malloc_size);  //make enough space
	int offset = 0;


	/* send the frame for encoding */
	ret = avcodec_send_frame(codecctx, frame);
	if (ret < 0) {
		fprintf(stderr, "Error sending the frame to the encoder\n");
		return false;
	}

	/* read all the available output packets (in general there may be any
	* number of them */
	while (ret >= 0) {
		ret = avcodec_receive_packet(codecctx, pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			break;
		else if (ret < 0) {
			fprintf(stderr, "Error encoding audio frame\n");
			return false;
		}

		if (offset + pkt->size > malloc_size) {
			fprintf(stderr, "Overflow of output size\n");
			return false;
		}
		memcpy(*data_out + 7 + offset, (char*)pkt->data, pkt->size);
		offset += pkt->size;
		av_packet_unref(pkt);
	}

	//av_frame_unref(frame);

	if (offset > 0) {
		size_out = offset + 7;
		addADTStoPacket(*data_out, size_out);
		return true;
	}

	return false;
}


bool AudioEncoderAAC::encode(AudioFramePtr &inFrame, AudioFramePtr &outFrame)
{
	int ret;

	ret = av_frame_make_writable(frame);
	if (ret < 0)
		return false;

	uint8_t *ins[2];
	ins[0] = (uint8_t*)inFrame->getData();
	ins[1] = (uint8_t*)inFrame->getData() + 2048;

	uint8_t *outs[2];
	outs[0] = (uint8_t*)frame->data[0];
	outs[1] = (uint8_t*)frame->data[1];

	int count = swr_convert(swr, outs, frame->nb_samples, (const uint8_t **)ins, frame->nb_samples);

	int malloc_size = inFrame->getSize() * 2;
	char *data_out = (char*)malloc(malloc_size);  //make enough space
	int offset = 0;


	/* send the frame for encoding */
	ret = avcodec_send_frame(codecctx, frame);
	if (ret < 0) {
		fprintf(stderr, "Error sending the frame to the encoder\n");
		return false;
	}

	/* read all the available output packets (in general there may be any
	* number of them */
	while (ret >= 0) {
		ret = avcodec_receive_packet(codecctx, pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			break;
		else if (ret < 0) {
			fprintf(stderr, "Error encoding audio frame\n");
			return false;
		}

		if (offset + pkt->size > malloc_size) {
			fprintf(stderr, "Overflow of output size\n");
			return false;
		}
		memcpy(data_out + 7 + offset, (char*)pkt->data, pkt->size);
		offset += pkt->size;
		av_packet_unref(pkt);
	}

	//av_frame_unref(frame);

	if (offset > 0) {
		int size_out = offset + 7;
		addADTStoPacket(data_out, size_out);

		outFrame = std::make_shared<AudioFrame>();
		outFrame->setData(data_out, size_out);
		return true;
	}

	return false;
}

