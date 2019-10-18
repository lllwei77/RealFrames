#include "AudioEncoderOpus.h"



AudioEncoderOpus::AudioEncoderOpus()
{
	frame = av_frame_alloc();
	pkt = av_packet_alloc();

	swr = swr_alloc();
	av_opt_set_int(swr, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
	av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
	av_opt_set_int(swr, "in_sample_rate", 44100, 0);
	av_opt_set_int(swr, "out_sample_rate", 48000, 0);
	av_opt_set_sample_fmt(swr, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
	av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
	swr_init(swr);
}


AudioEncoderOpus::~AudioEncoderOpus()
{
	av_frame_free(&frame);
	av_packet_free(&pkt);
	if (codecctx) 
		avcodec_free_context(&codecctx);

	swr_close(swr);
	swr_free(&swr);
}


bool AudioEncoderOpus::initialize()
{
	//codec = avcodec_find_encoder(AV_CODEC_ID_OPUS);
	codec = avcodec_find_encoder_by_name("libopus");
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
	codecctx->sample_fmt = AV_SAMPLE_FMT_FLT;  //for OPUS test
	codecctx->sample_rate = 48000;
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


int AudioEncoderOpus::getInputBuffSize()
{
	return codecctx->frame_size * 4;
}


bool AudioEncoderOpus::encode(char *data_in, int size_in, char **data_out, int &size_out)
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
	return false;
}


bool AudioEncoderOpus::encode(AudioFramePtr &inFrame, AudioFramePtr &outFrame)
{
	int ret;

	ret = av_frame_make_writable(frame);
	if (ret < 0)
		return false;

	uint8_t *ins[2];
	ins[0] = (uint8_t*)inFrame->getData();
	ins[1] = (uint8_t*)inFrame->getData() + 960*2;

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
		memcpy(data_out + offset, (char*)pkt->data, pkt->size);
		offset += pkt->size;
		av_packet_unref(pkt);
	}

	//av_frame_unref(frame);

	if (offset > 0) {
		int size_out = offset;

		outFrame = std::make_shared<AudioFrame>();
		outFrame->setData(data_out, size_out);
		return true;
	}

	return false;
}

