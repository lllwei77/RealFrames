#include "VideoInput.h"
#include "VideoFrame.h"

#include <assert.h>
#include <stdlib.h>



VideoInput::VideoInput(VideoDevicePtr VideoDevice, int width, int height):
	videoDevice(VideoDevice), width(width), height(height)
{
	captureQueue = new FrameQueue<VideoFramePtr>(6);
	swap_buff = (char*)malloc(buff_size);

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
}


VideoInput::~VideoInput()
{
	delete captureQueue;
	free(swap_buff);
}


bool VideoInput::open()
{
	avdevice_register_all();
	//av_log_set_level(AV_LOG_FATAL);

	pFormatCtx = avformat_alloc_context();
	AVInputFormat *pInputFormat = av_find_input_format("dshow");

	AVDictionary *options = NULL;
	av_dict_set(&options, "video_size", this->getResolution().c_str(), 0);

	int rc = avformat_open_input(&pFormatCtx, this->videoDevice->getFFName(), pInputFormat, &options);
	if (rc != 0) {
		av_dict_free(&options);
		return false;
	}

	av_dict_free(&options);

	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		printf("Couldn't find stream information.\n");
		return false;
	}

	videoindex = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}
	if (videoindex == -1) {
		printf("Didn't find a video stream.\n");
		return false;
	}

	pCodecCtx = avcodec_alloc_context3(NULL);
	if (pCodecCtx == NULL)
	{
		printf("Could not allocate AVCodecContext.\n");
		return false;
	}

	avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar);

	AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		printf("Codec not found.\n");
		return false;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Could not open codec.\n");
		return false;
	}

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	//pFrame = av_frame_alloc();
	//av_free(out_buffer);  crash.

	av_log_set_level(AV_LOG_FATAL);

	thread_stop = false;
	threadCapture = new thread(&VideoInput::procCapture, this);

	return true;
}


void VideoInput::close()
{
	thread_stop = true;
	threadCapture->join();
}


void VideoInput::procCapture()
{
	while (true) {

		int ret = av_read_frame(pFormatCtx, packet);
		if (ret < 0) {
			printf("read frame failed.\n");
			return;
		}

		if (packet->stream_index == videoindex) {
			ret = avcodec_send_packet(pCodecCtx, packet);
			if (ret < 0) {
				printf("Error submitting the packet to the decoder.\n");
				return;
			}

			AVFrame *pFrame = av_frame_alloc();

			ret = avcodec_receive_frame(pCodecCtx, pFrame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				return;
			else if (ret < 0) {
				printf("Error during decoding.\n");
				return;
			}

			AVFrame *pFrameYUV = av_frame_alloc();
			unsigned char *out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
			av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
				AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

			sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
				pFrameYUV->data, pFrameYUV->linesize);

			pFrameYUV->width = pFrame->width;
			pFrameYUV->height = pFrame->height;
			pFrameYUV->format = pFrame->format;

			av_frame_free(&pFrame);
			av_frame_unref(pFrame);
			av_packet_unref(packet);

			//水平翻转
			yuv_flip(*pFrameYUV->data, pFrameYUV->width, pFrameYUV->height);

			VideoFramePtr videoFrame = makeVideoFrame();
			videoFrame->setAvFrame(pFrameYUV);
			videoFrame->set_sws_scale_used();  //for patching memory leak~~~
			captureQueue->force_put(videoFrame);
		}

	}
}


void VideoInput::read(VideoFramePtr &VideoFrame)
{
	captureQueue->get(VideoFrame);
}


string VideoInput::getResolution()
{
	string resolution = to_string(width) + "x" + to_string(height);
	return resolution;
}


void VideoInput::yuv_rotate_180(uint8_t* yuvbuf, int width, int height) {

	uint8_t* dstbuf = new uint8_t[width*height * 3 / 2];
	int idx = 0;
	//旋转180:将右下角的点作为第一个点，从右往左，从下往上取点
	//Y 宽：[0,w-1]  高：[0,h-1]
	for (int i = height - 1; i >= 0; i--) {
		for (int j = width - 1; j >= 0; j--) {
			dstbuf[idx++] = *(yuvbuf + (i*width + j));
		}
	}
	uint8_t* uheader = yuvbuf + width * height;
	//U
	for (int i = height / 2 - 1; i >= 0; i--) {
		for (int j = width / 2 - 1; j >= 0; j--) {
			dstbuf[idx++] = *(uheader + (i*width / 2 + j));
		}
	}
	uint8_t* vheader = uheader + width * height / 4;
	//V
	for (int i = height / 2 - 1; i >= 0; i--) {
		for (int j = width / 2 - 1; j >= 0; j--) {
			dstbuf[idx++] = *(vheader + (i*width / 2 + j));
		}
	}

	memcpy(yuvbuf, dstbuf, width*height * 3 / 2);
	//delete[] yuvbuf;
	delete[] dstbuf;
}


//镜像翻转
void VideoInput::yuv_flip(uint8_t* yuvbuf, int width, int height) {

	uint8_t* dstbuf = new uint8_t[width*height * 3 / 2];
	int idx = 0;
	//水平翻转:将右上角的点作为第一个点，从右往左，从上往下取点
	//Y 宽：[0,w-1]  高：[0,h-1]
	for (int i = 0; i < height; i++) {
		for (int j = width - 1; j >= 0; j--) {
			dstbuf[idx++] = *(yuvbuf + (i*width + j));
		}
	}
	uint8_t* uheader = yuvbuf + width * height;
	//U
	for (int i = 0; i < height / 2; i++) {
		for (int j = width / 2 - 1; j >= 0; j--) {
			dstbuf[idx++] = *(uheader + (i*width / 2 + j));
		}
	}
	uint8_t* vheader = uheader + width * height / 4;
	//V
	for (int i = 0; i < height / 2; i++) {
		for (int j = width / 2 - 1; j >= 0; j--) {
			dstbuf[idx++] = *(vheader + (i*width / 2 + j));
		}
	}

	memcpy(yuvbuf, dstbuf, width*height * 3 / 2);
	//delete[] yuvbuf;
	delete[] dstbuf;
}
