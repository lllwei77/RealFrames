#include "AudioDecoder.h"



AudioDecoder::AudioDecoder()
{
	frame = av_frame_alloc();
	pkt = av_packet_alloc();

	swr = swr_alloc();
	av_opt_set_int(swr, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
	av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
	av_opt_set_int(swr, "in_sample_rate", 44100, 0);
	av_opt_set_int(swr, "out_sample_rate", 44100, 0);
	av_opt_set_sample_fmt(swr, "in_sample_fmt", AV_SAMPLE_FMT_FLTP, 0);
	av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16P, 0);
	int ret = swr_init(swr);
}


AudioDecoder::~AudioDecoder()
{
	av_frame_free(&frame);
	av_packet_free(&pkt);
	if (codecctx) avcodec_free_context(&codecctx);
}


bool AudioDecoder::initialize()
{
	codec = avcodec_find_decoder(AV_CODEC_ID_AAC);

	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		return false;
	}

	parser = av_parser_init(codec->id);
	if (!parser) {
		fprintf(stderr, "Parser not found\n");
		return false;
	}

	codecctx = avcodec_alloc_context3(codec);
	if (!codecctx) {
		fprintf(stderr, "Could not allocate audio codec context\n");
		return false;
	}

	if (avcodec_open2(codecctx, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		return false;
	}

	return true;
}


bool AudioDecoder::decode(char *data_in, int size_in, char **data_out, int &size_out)
{
	int ret;
	int offset = 0;
	int parsed = 0;
	int parse_left = size_in;
	char *parse_data = data_in;
	int samples_num = 0;

	while (parse_left > 0) {

		parse_data += (size_in - parse_left);

		parsed = av_parser_parse2(parser, codecctx, &pkt->data, &pkt->size,
			(uint8_t*)parse_data, parse_left,
			AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

		if (parsed < 0) {
			fprintf(stderr, "Error while parsing\n");
			return false;
		}

		parse_left -= parsed;

		int i, ch;
		int data_size;

		/* send the packet with the compressed data to the decoder */
		ret = avcodec_send_packet(codecctx, pkt);
		if (ret < 0) {
			fprintf(stderr, "Error submitting the packet to the decoder\n");
			return false;
		}

		/* read all the output frames (in general there may be any number of them */
		while (ret >= 0) {
			ret = avcodec_receive_frame(codecctx, frame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				break;
			else if (ret < 0) {
				fprintf(stderr, "Error during decoding\n");
				return false;
			}

			data_size = av_get_bytes_per_sample(codecctx->sample_fmt);
			if (data_size < 0) {
				/* This should not occur, checking just for paranoia */
				fprintf(stderr, "Failed to calculate data size\n");
				return false;
			}

			for (i = 0; i < frame->nb_samples; i++) {
				for (ch = 0; ch < codecctx->channels; ch++) {
					//fwrite(frame->data[ch] + data_size*i, 1, data_size, outfile);
					memcpy(temp_data + offset, frame->data[ch] + data_size * i, data_size);
					offset += data_size;
				}
			}
			samples_num += frame->nb_samples;
			av_frame_unref(frame);
		}
	}


	uint8_t *ins[2];
	ins[0] = (uint8_t*)temp_data;
	ins[1] = (uint8_t*)(temp_data + (offset / 2));

	*data_out = (char*)malloc(offset / 2);
	memset(*data_out, 0, offset / 2);

	uint8_t *outs[2];
	outs[0] = (uint8_t*)(*data_out);
	outs[1] = (uint8_t*)(*data_out + offset / 4);

	int count = swr_convert(swr, (uint8_t**)outs, samples_num, (const uint8_t **)ins, samples_num);
	if (count == 0)
		return false;

	size_out = count * 2 * 2;  //2 channels * 2 bytes

	/*
	*data_out = (char*)malloc(offset);
	memcpy(*data_out, temp_data, offset);
	size_out = offset;
	*/

	return true;
}


bool AudioDecoder::decode(AudioFramePtr &inFrame, AudioFramePtr &outFrame)
{
	int ret;
	int offset = 0;
	int parsed = 0;
	int parse_left = inFrame->getSize();
	char *parse_data = inFrame->getData();
	int samples_num = 0;

	while (parse_left > 0) {

		parse_data += (inFrame->getSize() - parse_left);

		parsed = av_parser_parse2(parser, codecctx, &pkt->data, &pkt->size,
			(uint8_t*)parse_data, parse_left,
			AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

		if (parsed < 0) {
			fprintf(stderr, "Error while parsing\n");
			return false;
		}

		parse_left -= parsed;

		int i, ch;
		int data_size;

		/* send the packet with the compressed data to the decoder */
		ret = avcodec_send_packet(codecctx, pkt);
		if (ret < 0) {
			fprintf(stderr, "Error submitting the packet to the decoder\n");
			return false;
		}

		/* read all the output frames (in general there may be any number of them */
		while (ret >= 0) {
			ret = avcodec_receive_frame(codecctx, frame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				break;
			else if (ret < 0) {
				fprintf(stderr, "Error during decoding\n");
				return false;
			}

			data_size = av_get_bytes_per_sample(codecctx->sample_fmt);
			if (data_size < 0) {
				/* This should not occur, checking just for paranoia */
				fprintf(stderr, "Failed to calculate data size\n");
				return false;
			}

			for (i = 0; i < frame->nb_samples; i++) {
				for (ch = 0; ch < codecctx->channels; ch++) {
					//fwrite(frame->data[ch] + data_size*i, 1, data_size, outfile);
					memcpy(temp_data + offset, frame->data[ch] + data_size * i, data_size);
					offset += data_size;
				}
			}
			samples_num += frame->nb_samples;
			av_frame_unref(frame);
		}
	}


	uint8_t *ins[2];
	ins[0] = (uint8_t*)temp_data;
	ins[1] = (uint8_t*)(temp_data + (offset / 2));

	char *data_out = (char*)malloc(offset / 2);
	memset(data_out, 0, offset / 2);

	uint8_t *outs[2];
	outs[0] = (uint8_t*)(data_out);
	outs[1] = (uint8_t*)(data_out + offset / 4);

	int count = swr_convert(swr, (uint8_t**)outs, samples_num, (const uint8_t **)ins, samples_num);
	if (count == 0)
		return false;

	int size_out = count * 2 * 2;  //2 channels * 2 bytes

	/*
	*data_out = (char*)malloc(offset);
	memcpy(*data_out, temp_data, offset);
	size_out = offset;
	*/
	outFrame = makeAudioFrame();
	outFrame->setData(data_out, size_out);

	return true;
}

