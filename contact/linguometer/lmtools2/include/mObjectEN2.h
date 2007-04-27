/* Coded by M. Tavella
 * email michele@liralab.it
 * web   http://michele.dnsalias.org
 */

#ifndef MOBJECTEN2_H
#define MOBJECTEN2_H

#ifdef FFMPEG_DISTRO
#	include <ffmpeg/avcodec.h>
#	include <ffmpeg/avformat.h>
#else
#	include "avcodec.h"
#	include "avformat.h"
#endif

#include "mInclude.h"
#include "mClass.h"
#include "mObjectPCM.h"
#include "mObjectDV2Tools.h"

#define STREAM_DURATION   50.0
#define STREAM_FRAME_RATE 25 // 25 images/s
#define STREAM_NB_FRAMES  ((int)(STREAM_DURATION * STREAM_FRAME_RATE))
#define STREAM_PIX_FMT PIX_FMT_YUV420P // default pix_fmt
//static int sws_flags = SWS_BICUBIC;



class mObjectEN2 : public mClass {
	public:
	mObjectEN2();
	
	bool setFile(char *filename);
	bool openFile(void);
	bool closeFile(void);
	
	bool setAudio(unsigned int sampleRate,
			unsigned int channels, unsigned int bitRate = 64000);
	
	bool hasVideo(void);
	bool hasAudio(void);

	int64_t getVideoPTS(void);
	int64_t getAudioPTS(void);
	
	bool showProgress (int64_t pts);

	bool encodeAudio(void);
	bool encodeVideo(void);

	int addAudioFrame(int audioFrames,
			int16_t *bufferL, int16_t *bufferR, unsigned int samples);
	bool writeAudioFrame(void);
	
	bool isDone(unsigned int audioSamples, unsigned int videoFrames = 0);
	bool setFormat(void);
	bool setStreams(bool verbose = true);

	private:

	void openAudio(void);
	void closeAudio(void);
	
	AVStream * addAudioStream(int codec_id);
	
	void openVideo(void);
	void closeVideo(void);


	// ============================================================================= //
	AVStream *add_video_stream(int codec_id);
	AVFrame *alloc_picture(int pix_fmt, int width, int height);
	//void open_video(void);
	void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height);
	void write_video_frame(AVFormatContext *oc, AVStream *st);
	
	private:
	// ============================================================================= //
	char *_filename;
    AVOutputFormat *_outputFmt;
	AVFormatContext *_formatCtx;
    AVStream *_streamAudio;
	AVStream *_streamVideo;

	unsigned int _audioBitRate;
	unsigned int _audioSampleRate;
	unsigned int _audioChannels;
	unsigned int _audioSamples;
	// ============================================================================= //
	
	// Audio stuff
	float t, tincr, tincr2;
	int16_t *_audioFrameBuffer;
	uint8_t *_audioWriteBuffer;
	int _audioWriteSize;
	int _audioFrameSamples;

	// Video stuff
	AVFrame *picture, *tmp_picture;
	uint8_t *video_outbuf;
	int frame_count, video_outbuf_size;
};


mObjectEN2::mObjectEN2(void) {
	SetName("mObjectEN2");

	_outputFmt = NULL;
	_formatCtx = NULL;
	_streamAudio = NULL;
	_streamVideo = NULL;
}

bool mObjectEN2::setFile(char *filename) {
	if (filename == NULL)
		return false;
	
	//if (_filename != NULL)
	//	delete _filename;
	//else
		_filename = new char [LMTOOLS_FNSIZE];

	if (!strcpy(_filename, filename))
		return false;
	
	return true;
}

bool mObjectEN2::setFormat(void) {
	/*
	if (_outputFmt != NULL)
		delete _outputFmt;
	if (_formatCtx != NULL)
		delete _formatCtx;
	*/
    av_register_all();
	
	_outputFmt = guess_format(NULL, _filename, NULL);
	if (!_outputFmt) { 
        printf("Could not deduce output format from file extension: using MPEG.\n");
        _outputFmt = guess_format("mpeg", NULL, NULL);
	}
    if (!_outputFmt) 
		return false;
   	
	_formatCtx = av_alloc_format_context();
    if (!_formatCtx) 
		return false;

	_formatCtx->oformat = _outputFmt;

	return true;	
}

bool mObjectEN2::setStreams(bool verbose) {
	/*
	if (_streamAudio != NULL)
		delete _streamAudio;
	if (_streamVideo != NULL)
		delete _streamVideo;
	*/
	if (_outputFmt->video_codec != CODEC_ID_NONE)
		_streamVideo = add_video_stream (_outputFmt->video_codec);
	if (_outputFmt->audio_codec != CODEC_ID_NONE)
		_streamAudio = addAudioStream (_outputFmt->audio_codec);

    if (av_set_parameters(_formatCtx, NULL) < 0)
		return false;
    
	if (verbose)
		dump_format(_formatCtx, 0, _filename, 1);
    
	if (_streamVideo)
        openVideo();
    if (_streamAudio)
        openAudio();


	return true;	
}


bool mObjectEN2::openFile(void) {
	if (!setFormat() || !setStreams())
		return false;

    if (!(_outputFmt->flags & AVFMT_NOFILE)) {
        if (url_fopen(&_formatCtx->pb, _filename, URL_WRONLY) < 0)
			return false;
    }
	// Segfaults
    av_write_header(_formatCtx);

	return true;
}


bool mObjectEN2::closeFile(void) {
	if (_streamVideo)
		closeVideo();
	if (_streamAudio)
		closeAudio();
    
    av_write_trailer(_formatCtx);

    for(unsigned int i = 0; i < _formatCtx->nb_streams; i++) {
        av_freep(&_formatCtx->streams[i]->codec);
        av_freep(&_formatCtx->streams[i]);
    }

    if (!(_outputFmt->flags & AVFMT_NOFILE)) {
        url_fclose(&_formatCtx->pb);
    }

    av_free(_formatCtx);

	return true;
}

int64_t mObjectEN2::getVideoPTS (void) {
	if (_streamVideo) 
		return av_rescale_q (_streamVideo->pts.val, 
				_streamVideo->time_base, 
				AV_TIME_BASE_Q);
	else
		return 0;
	
}
int64_t mObjectEN2::getAudioPTS (void) {
	if (_streamAudio) 
		return av_rescale_q (_streamAudio->pts.val, 
				_streamAudio->time_base, 
				AV_TIME_BASE_Q);
	else
		return 0;
}

bool mObjectEN2::showProgress (int64_t pts) {
	int hours = 0, mins = 0, 
			   secs = 0, us = 0;

	if (!mObjectDV2Tools::pts2time(pts, hours, mins, secs, us))
		return false;

	fprintf(stderr, " Encoding at time: %02d:%02d:%02d.%01d  \r",
		hours, mins, secs, (10 * us) / AV_TIME_BASE);
	fflush(stderr);
	return true;
}

bool mObjectEN2::hasVideo(void) {
	return _streamVideo > 0 ? true : false;
};

bool mObjectEN2::hasAudio(void) {
	return _streamAudio > 0 ? true : false; 
};

bool mObjectEN2::setAudio (unsigned int sampleRate,
		unsigned int channels, unsigned int bitRate) {
	if (sampleRate <= 0 || bitRate <= 0 || channels < 1 || channels > 2)
		return false;
	
	_audioBitRate = bitRate;
	_audioSampleRate = sampleRate;
	_audioChannels = channels;
	return true;
}
	
bool mObjectEN2::encodeAudio(void) {
	double audioPTS = getAudioPTS();
	double videoPTS = getVideoPTS();
	return (!_streamVideo || 
			(_streamVideo && _streamAudio && audioPTS < videoPTS));
}

bool mObjectEN2::encodeVideo(void) {
	return !encodeAudio();
}

AVStream * mObjectEN2::addAudioStream(int codec_id)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(_formatCtx, 1);
    if (!st) {
        fprintf(stderr, "Could not alloc stream\n");
        exit(1);
    }

    c = st->codec;
    c->codec_id = (CodecID)codec_id;
    c->codec_type = CODEC_TYPE_AUDIO;

    /* put sample parameters 
    c->bit_rate = 64000;
    c->sample_rate = 44100;
    c->channels = 2;
   */ 
	
	c->bit_rate = _audioBitRate;
    c->sample_rate = _audioSampleRate;
    c->channels = _audioChannels;
    return st;
}

void mObjectEN2::openAudio(void)
{
    AVCodecContext *c;
    AVCodec *codec;

    c = _streamAudio->codec;

    /* find the audio encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        exit(1);
    }

    /* open it */
    if (avcodec_open(c, codec) < 0) {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }

    _audioWriteSize = 10000;
    _audioWriteBuffer = (uint8_t*)av_malloc(_audioWriteSize);

    /* ugly hack for PCM codecs (will be removed ASAP with new PCM
       support to compute the input frame size in samples */
	if (c->frame_size <= 1) {
		_audioFrameSamples = _audioWriteSize / c->channels;
		switch(_streamAudio->codec->codec_id) {
			case CODEC_ID_PCM_S16LE:
			case CODEC_ID_PCM_S16BE:
			case CODEC_ID_PCM_U16LE:
			case CODEC_ID_PCM_U16BE:
				_audioFrameSamples >>= 1;
				break;
			default:
				break;
		}
	}
	else 
		_audioFrameSamples = c->frame_size;
    
	_audioFrameBuffer = (int16_t*)av_malloc(_audioFrameSamples * 2 * c->channels);
}

int mObjectEN2::addAudioFrame(int audioFrames,
		int16_t *bufferL, int16_t *bufferR, unsigned int samples) {
	/* No buffers */
	if (bufferL == NULL && bufferR)
		return -1;
	/* Error */
	if (bufferL == NULL && bufferR != NULL)
		return -1;

	unsigned int ds = _audioFrameSamples * audioFrames;

	if (bufferL != NULL && bufferR == NULL) {
		if (_audioFrameSamples + ds > samples)
			memset(_audioFrameBuffer, 0, _audioFrameSamples * sizeof(int16_t));
		for (unsigned int s = 0; s < (unsigned int)_audioFrameSamples; s++) {
			if (s + ds > samples) 
				break;
			_audioFrameBuffer [s] = bufferL [s + ds];
		}
	}
	if (bufferL != NULL && bufferR != NULL) {
		if (_audioFrameSamples + ds > samples)
			memset(_audioFrameBuffer, 0, 2 *_audioFrameSamples * sizeof(int16_t));
		for (unsigned int s = 0; s < (unsigned int)_audioFrameSamples; s++) {
			if (s + ds > samples) 
				break;
			_audioFrameBuffer [2*s + 0] = bufferL [s + ds];
			_audioFrameBuffer [2*s + 1] = bufferR [s + ds];
		}
	}
	return ++audioFrames;
}

bool mObjectEN2::writeAudioFrame(void)
{
    AVCodecContext *c;
    AVPacket pkt;
    av_init_packet(&pkt);

    c = _streamAudio->codec;

    pkt.size= avcodec_encode_audio(c, _audioWriteBuffer, _audioWriteSize, _audioFrameBuffer);

    pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, _streamAudio->time_base);
    pkt.flags |= PKT_FLAG_KEY;
    pkt.stream_index= _streamAudio->index;
    pkt.data= _audioWriteBuffer;

    if (av_write_frame(_formatCtx, &pkt) != 0) 
    	return false;
	return true;
}

bool mObjectEN2::isDone(unsigned int audioSamples, unsigned int videoFrames) {
	int64_t audioDuration = (int64_t)((double)audioSamples / (double)_audioSampleRate
			* AV_TIME_BASE);
	int64_t videoDuration = videoFrames  / 25 * AV_TIME_BASE;
	
	
	if (!_streamVideo)
		return getAudioPTS() >= audioDuration;
	else
		return getVideoPTS() >= videoDuration;
}


void mObjectEN2::closeAudio(void)
{
    avcodec_close(_streamAudio->codec);

    av_free(_audioFrameBuffer);
    av_free(_audioWriteBuffer);
}

AVStream *mObjectEN2::add_video_stream(int codec_id)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(_formatCtx, 0);
    if (!st) {
        fprintf(stderr, "Could not alloc stream\n");
        exit(1);
    }

    c = st->codec;
    c->codec_id = (CodecID)codec_id;
    c->codec_type = CODEC_TYPE_VIDEO;

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = 352;
    c->height = 288;
    /* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
    c->time_base.den = STREAM_FRAME_RATE;
    c->time_base.num = 1;
    c->gop_size = 12; /* emit one intra frame every twelve frames at most */
    c->pix_fmt = STREAM_PIX_FMT;
    if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
        /* just for testing, we also add B frames */
        c->max_b_frames = 2;
    }
    if (c->codec_id == CODEC_ID_MPEG1VIDEO){
        /* needed to avoid using macroblocks in which some coeffs overflow
           this doesnt happen with normal video, it just happens here as the
           motion of the chroma plane doesnt match the luma plane */
        c->mb_decision=2;
    }
    // some formats want stream headers to be seperate
    if(!strcmp(_formatCtx->oformat->name, "mp4") || !strcmp(_formatCtx->oformat->name, "mov") || !strcmp(_formatCtx->oformat->name, "3gp"))
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    return st;
}

AVFrame *mObjectEN2::alloc_picture(int pix_fmt, int width, int height)
{
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = avcodec_alloc_frame();
    if (!picture)
        return NULL;
    size = avpicture_get_size(pix_fmt, width, height);
    picture_buf = (uint8_t*)av_malloc(size);
    if (!picture_buf) {
        av_free(picture);
        return NULL;
    }
    avpicture_fill((AVPicture *)picture, picture_buf,
                   pix_fmt, width, height);
    return picture;
}

void mObjectEN2::openVideo(void)
{
    AVCodec *codec;
    AVCodecContext *c;

    c = _streamVideo->codec;

    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        exit(1);
    }

    /* open the codec */
    if (avcodec_open(c, codec) < 0) {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }

    video_outbuf = NULL;
    if (!(_formatCtx->oformat->flags & AVFMT_RAWPICTURE)) {
        /* allocate output buffer */
        /* XXX: API change will be done */
        /* buffers passed into lav* can be allocated any way you prefer,
           as long as they're aligned enough for the architecture, and
           they're freed appropriately (such as using av_free for buffers
           allocated with av_malloc) */
        video_outbuf_size = 200000;
        video_outbuf = (uint8_t*)av_malloc(video_outbuf_size);
    }

    /* allocate the encoded raw picture */
    picture = alloc_picture(c->pix_fmt, c->width, c->height);
    if (!picture) {
        fprintf(stderr, "Could not allocate picture\n");
        exit(1);
    }

    /* if the output format is not YUV420P, then a temporary YUV420P
       picture is needed too. It is then converted to the required
       output format */
    tmp_picture = NULL;
    if (c->pix_fmt != PIX_FMT_YUV420P) {
        tmp_picture = alloc_picture(PIX_FMT_YUV420P, c->width, c->height);
        if (!tmp_picture) {
            fprintf(stderr, "Could not allocate temporary picture\n");
            exit(1);
        }
    }
}

/* prepare a dummy image */
void mObjectEN2::fill_yuv_image(AVFrame *pict, int frame_index, int width, int height)
{
    int x, y, i;

    i = frame_index;

    /* Y */
    for(y=0;y<height;y++) {
        for(x=0;x<width;x++) {
            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;
        }
    }

    /* Cb and Cr */
    for(y=0;y<height/2;y++) {
        for(x=0;x<width/2;x++) {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }
}

void mObjectEN2::write_video_frame(AVFormatContext *oc, AVStream *st)
{
    int out_size, ret;
    AVCodecContext *c;
    static struct SwsContext *img_convert_ctx;

    c = st->codec;

    if (frame_count >= STREAM_NB_FRAMES) {
        /* no more frame to compress. The codec has a latency of a few
           frames if using B frames, so we get the last frames by
           passing the same picture again */
    } else {
        if (c->pix_fmt != PIX_FMT_YUV420P) {
            /* as we only generate a YUV420P picture, we must convert it
               to the codec pixel format if needed */
            if (img_convert_ctx == NULL) {
				int a = 0;
				a = a + 10;
				/*
                img_convert_ctx = sws_getContext(c->width, c->height,
                                                 PIX_FMT_YUV420P,
                                                 c->width, c->height,
                                                 c->pix_fmt,
                                                 sws_flags, NULL, NULL, NULL);
												 */
                if (img_convert_ctx == NULL) {
                    fprintf(stderr, "Cannot initialize the conversion context\n");
                    exit(1);
                }
            }
            fill_yuv_image(tmp_picture, frame_count, c->width, c->height);
			/*
            sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize,
                      0, c->height, picture->data, picture->linesize);
					  */
        } else {
            fill_yuv_image(picture, frame_count, c->width, c->height);
        }
    }


    if (oc->oformat->flags & AVFMT_RAWPICTURE) {
        /* raw video case. The API will change slightly in the near
           futur for that */
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags |= PKT_FLAG_KEY;
        pkt.stream_index= st->index;
        pkt.data= (uint8_t *)picture;
        pkt.size= sizeof(AVPicture);

        ret = av_write_frame(oc, &pkt);
    } else {
        /* encode the image */
        out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);
        /* if zero size, it means the image was buffered */
        if (out_size > 0) {
            AVPacket pkt;
            av_init_packet(&pkt);

            pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
            if(c->coded_frame->key_frame)
                pkt.flags |= PKT_FLAG_KEY;
            pkt.stream_index= st->index;
            pkt.data= video_outbuf;
            pkt.size= out_size;

            /* write the compressed frame in the media file */
            ret = av_write_frame(oc, &pkt);
        } else {
            ret = 0;
        }
    }
    if (ret != 0) {
        fprintf(stderr, "Error while writing video frame\n");
        exit(1);
    }
    frame_count++;
}

void mObjectEN2::closeVideo()
{
    avcodec_close(_streamVideo->codec);
    av_free(picture->data[0]);
    av_free(picture);
    if (tmp_picture) {
        av_free(tmp_picture->data[0]);
        av_free(tmp_picture);
    }
    av_free(video_outbuf);
}


void mEncodePCM16 (char *filename,
		int16_t *bufferL, int16_t *bufferR, 
		unsigned int samples, unsigned int bitRate, unsigned int channels) {

	mObjectEN2 *encoder = new mObjectEN2;
	encoder->setFile(filename);
	encoder->setAudio(bitRate, channels);
	encoder->openFile();
	
	int audioFrames = 0;
	while(1) {
		if (encoder->encodeAudio()) {
			audioFrames = encoder->addAudioFrame(audioFrames, bufferL, bufferR, samples);
			assert (encoder->writeAudioFrame());
		}
		encoder->showProgress(encoder->getAudioPTS());
		if (encoder->isDone(samples))
			break;
	}
	encoder->closeFile();
	cout << endl;
}
#endif
