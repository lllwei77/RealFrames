#include "VideoEncoderH265.h"
#include<winsock2.h>


VideoEncoderH265::VideoEncoderH265()
{
	//pFrame = av_frame_alloc();
	pFormatCtx = avformat_alloc_context();
}


VideoEncoderH265::~VideoEncoderH265()
{
}


bool VideoEncoderH265::init(int width, int height)
{
	int in_w = width, in_h = height;

	pCodec = avcodec_find_encoder(AV_CODEC_ID_HEVC);
	pCodecCtx = avcodec_alloc_context3(pCodec);

	////
	pCodecCtx->codec_id = pCodec->id;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;
	pCodecCtx->width = in_w;
	pCodecCtx->height = in_h;
	pCodecCtx->bit_rate = 64000;
	pCodecCtx->gop_size = 300;
	////

	AVDictionary *options = NULL;
	if (pCodecCtx->codec_id == AV_CODEC_ID_H265) {
		av_dict_set(&options, "x265-params", "qp=20", 0);
		av_dict_set(&options, "preset", "ultrafast", 0);
		av_dict_set(&options, "tune", "zero-latency", 0);
	}

	if (avcodec_open2(pCodecCtx, pCodec, &options) < 0) {
		printf("Failed to open encoder! \n");
		return false;
	}
	
	return true;
}


AVPacket* VideoEncoderH265::encode(AVFrame *pFrame, int &i_counter, int &p_counter)
{
	AVPacket *pPacket = av_packet_alloc();

	if (ins_p_counter >= this->set_I_interval) {
		pFrame->pict_type = AV_PICTURE_TYPE_I;
	}
	else {
		pFrame->pict_type = AV_PICTURE_TYPE_P;   //no B frame
	}
	
	int ret;
	ret = avcodec_send_frame(pCodecCtx, pFrame);
	if (ret < 0) {
		if (AVERROR(EINVAL) == ret)
			printf("codec error\n");

		fprintf(stderr, "Error sending the frame to the encoder\n");
		return NULL;
	}

	//if (pFrame->pict_type == AV_PICTURE_TYPE_I) {   not work!!
	//	printf("this is an I frame.\n");
	//}

	ret = avcodec_receive_packet(pCodecCtx, pPacket);
	if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		return NULL;
	else if (ret < 0) {
		fprintf(stderr, "Error encoding audio frame\n");
		return NULL;
	}

	if (pPacket->flags & AV_PKT_FLAG_CORRUPT) {
		return NULL;
	}

	if ((pPacket->flags & AV_PKT_FLAG_KEY) || (pFrame->pict_type == AV_PICTURE_TYPE_I)) {    //worked :)
		//printf("this is an I frame.\n");
		if (ins_i_counter > 10000)
			ins_i_counter = 0;
		else
			ins_i_counter++;

		ins_p_counter = 0;
	}
	else {
		ins_p_counter++;
	}

	//i_counter = htons(ins_i_counter);
	//p_counter = htons(ins_p_counter);
	i_counter = ins_i_counter;
	p_counter = ins_p_counter;

	//av_packet_unref(pPacket);
	return pPacket;
}


bool VideoEncoderH265::encode(VideoFramePtr &inFrame, VideoFramePtr &outFrame)
{
	int i_counter, p_counter;
	AVPacket *packet = encode(inFrame->getAvFrame(), i_counter, p_counter);
	if (NULL == packet)
	{
		return false;
	}

	outFrame = std::make_shared<VideoFrame>();
	outFrame->setAvPacket(packet);
	outFrame->set_i_counter(i_counter);
	outFrame->set_p_counter(p_counter);
	return true;
}

