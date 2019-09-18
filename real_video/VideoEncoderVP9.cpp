#include "VideoEncoderVP9.h"
#include<winsock2.h>


VideoEncoderVP9::VideoEncoderVP9()
{
	//pFrame = av_frame_alloc();
	pFormatCtx = avformat_alloc_context();
}


VideoEncoderVP9::~VideoEncoderVP9()
{
}


bool VideoEncoderVP9::init(int width, int height)
{
	int in_w = width, in_h = height;

	pCodec = avcodec_find_encoder(AV_CODEC_ID_VP9);
	pCodecCtx = avcodec_alloc_context3(pCodec);

	////
	pCodecCtx->codec_id = pCodec->id;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;
	pCodecCtx->width = in_w;
	pCodecCtx->height = in_h;
	//pCodecCtx->bit_rate = 64000;
	pCodecCtx->bit_rate = 64000;
	pCodecCtx->gop_size = 300;
	////

	/*
	
	    else if (codecVP9)
    { 
        // Setting explanation:
        // http://wiki.webmproject.org/ffmpeg/vp9-encoding-guide
        // https://developers.google.com/media/vp9/live-encoding/
        // https://developers.google.com/media/vp9/settings/vod/
        // https://developers.google.com/media/vp9/bitrate-modes/

        if(reduceQuality) //crf is the quality value for VP9 (0-63), lower the better quality youll get 
          av_dict_set(&opt, "crf"    ,      "40",         0); 
        else
          av_dict_set(&opt, "crf"    ,      "45",         0);
        av_dict_set(&opt, "speed"  ,        "8" ,         0);
        av_dict_set(&opt, "quality",        "realtime",   0); //realtime is recommended
        //av_dict_set(&opt, "threads",        "4" ,         0);
        //av_dict_set(&opt, "tile-columns",   "2",          0);
        //av_dict_set(&opt, "frame-parallel", "1",          0);
        //av_dict_set(&opt, "row-mt",         "1",          0);
        av_dict_set(&opt, "b:v",            "1",         0);
        av_dict_set(&opt, "g",              "400",        0); //key frame interval (big difference in vid size)

        //av_dict_set(&opt, "maxrate",        "3k",         0);
        //av_dict_set(&opt, "minrate",        "1k",         0);
        //av_dict_set(&opt, "hwaccel",        "vaapi",      0);
        //av_dict_set(&opt, "c:v",            "libvpx-vp9", 0);


		Most of the current VP9 decoders use tile-based, multi-threaded decoding. In order for the decoders to take advantage of multiple cores, the encoder must set tile-columns and frame-parallel.
    }
	*/


	AVDictionary *options = NULL;
	if (pCodecCtx->codec_id == AV_CODEC_ID_VP9) {
		av_dict_set(&options, "quality", "good", 0);  //good, best, realtime
		av_dict_set(&options, "speed", "6", 0);
		av_dict_set(&options, "slices", "4", 0);
		av_dict_set(&options, "threads", "4", 0);
		av_dict_set(&options, "g", "20", 0);   //key frame interval
		av_dict_set(&options, "minrate", "500k", 0);
		av_dict_set(&options, "maxrate", "1000k", 0);
		av_dict_set(&options, "hwaccel", "vaapi", 0);
		av_dict_set(&options, "lag-in-frames", "0", 0);      //! zero-latency!!!
	}

	if (avcodec_open2(pCodecCtx, pCodec, &options) < 0) {
		printf("Failed to open encoder! \n");
		return false;
	}
	
	return true;
}


AVPacket* VideoEncoderVP9::encode(AVFrame *pFrame, int &i_counter, int &p_counter)
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


bool VideoEncoderVP9::encode(VideoFramePtr &inFrame, VideoFramePtr &outFrame)
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

