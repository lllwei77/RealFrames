#include "VideoDecoder.h"
#include<winsock2.h>



VideoDecoder::VideoDecoder()
{
	pFormatCtx = avformat_alloc_context();
}


VideoDecoder::~VideoDecoder()
{

}


bool VideoDecoder::init()
{
	pCodec = avcodec_find_decoder(AV_CODEC_ID_HEVC);
	pCodecCtx = avcodec_alloc_context3(pCodec);

	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Failed to open encoder! \n");
		return false;
	}

	parser = av_parser_init(pCodec->id);

	return true;
}


AVFrame* VideoDecoder::decode(AVPacket *pPacket, int i_counter, int p_counter)
{
	AVFrame* pFrame = av_frame_alloc();

	if (0 == p_counter) {  //this is an I frame
		ins_i_counter = i_counter;
		ins_p_counter = 0;
	}
	else if (i_counter != ins_i_counter) {  //not in a same I frame
		return NULL;
	}
	//else if (*p_counter <= ins_p_counter) {  //wrong P frame sequence
	else if (p_counter - ins_p_counter != 1) {  //wrong P frame sequence
		return NULL;
	}
	else {
		ins_p_counter = p_counter;
	}

	int ret;
	ret = avcodec_send_packet(pCodecCtx, pPacket);
	if (ret < 0) {
		if (AVERROR(EINVAL) == ret)
			printf("error EINVAL\n");

		fprintf(stderr, "Error submitting the packet to the decoder\n");
		return NULL;
	}

	ret = avcodec_receive_frame(pCodecCtx, pFrame);
	if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		return NULL;
	else if (ret < 0) {
		fprintf(stderr, "Error during decoding\n");
		return NULL;
	}

	//av_frame_unref(pFrame);
	return pFrame;
}


bool VideoDecoder::decode(VideoFramePtr &inFrame, VideoFramePtr &outFrame)
{

	AVFrame *frame = decode(inFrame->getAvPacket(), inFrame->get_i_counter(), inFrame->get_p_counter());
	if (NULL == frame)
	{
		return false;
	}

	outFrame = makeVideoFrame();
	outFrame->setAvFrame(frame);

	return true;
}

