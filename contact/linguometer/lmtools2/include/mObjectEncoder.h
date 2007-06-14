/*
 *  Copyright (C) 2007 Michele Tavella <michele@liralab.it>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef MOBJENCODER_H
#define MOBJENCODER_H

/*
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
*/

#ifdef FFMPEG_DISTRO
#	include <ffmpeg/avcodec.h>
#	include <ffmpeg/avformat.h>
#else
#	include "avcodec.h"
#	include "avformat.h"
#endif

#include "mClass.h"

/* Those should be removed... 
 */
#define STREAM_DURATION   12
#define STREAM_FRAME_RATE 25 
#define STREAM_NB_FRAMES  ((int)(STREAM_DURATION * STREAM_FRAME_RATE))
#define STREAM_PIX_FMT PIX_FMT_YUV420P 


/* mObjectEncoder could be considered
 * a quick hack to FFMPEG's output_example.c
 * It has been altered to support plain PCM 
 * streams.
 */
class mObjectEncoder : public mClass
{
	public:
		mObjectEncoder (void);

	public:
		void Init (void);
		bool EncodeAudio (char *filename = "audiodump.wav");
		void SetupAudioIn (int16_t *buffer, int16_t *bufferR, int samples, int sampleRate = 48000, int channels = 2);
		void SetupAudioOut (int bitRate = 64000, int sampleRate = 48000, int channels = 2);
		void ShowAudioProgress (double nowPTS, double totPTS);

	/*
	 * Audio Encoding 
	 */
	private:
		AVStream *AddAudioStream (int codecID);
		void OpenAudio (AVStream *stream);
		inline void GetAudioFrame (int frame_size);
		inline void WriteAudioFrame (AVStream *stream);
		void CloseAudio(AVStream *stream);

	private:
    	AVFormatContext *_formatCtx;

		int16_t *_audioOutBufferLR;
		uint8_t *_audioOutBuffer;
		// Output Buffer Size (in samples)
		int _audioOutSize;
		// Input Frame Size (in samples)
		int _audioInSize;
		int16_t *_bufferL;
		int16_t *_bufferR;
	
		/* Audio Input vars 
		 * Those describe the input PCM
		 * stream.
		 */
		bool _audioInOK;
		int _audioInSampleRate;
		int _audioInSamples;
		int _audioInChannels;
		int16_t *_audioInBufferL;
		int16_t *_audioInBufferR;
		double _audioInSecs;

		/* Audio Output vars 
		 * Those describe the output
		 * stream.
		 */
		bool _audioOutOK;
		int _audioOutBitRate;
		int _audioOutSampleRate;
		int _audioOutChannels;


	/*
	 * Video Encoding 
	 */
	private:
		AVStream *AddVideoStream(AVFormatContext *_formatCtx, int codecID);
		AVFrame *AllocPicture (int pix_fmt, int width, int height);
		void OpenVideo (AVFormatContext *_formatCtx, AVStream *stream);
		void FillYUVImage (AVFrame *pict, int frame_index, int width, int height);
		void WriteVideoFrame (AVFormatContext *_formatCtx, AVStream *stream);
		void CloseVideo (AVFormatContext *_formatCtx, AVStream *stream);
	
	private:
		AVFrame *picture;
		AVFrame *tmp_picture;
		uint8_t *video_outbuf;
		int frame_count;
		int video_outbuf_size;
};

		
mObjectEncoder::mObjectEncoder (void)
{
	SetName ("mObjEncoder");
	Init ();	
}
	
void mObjectEncoder::Init (void)
{
	// Audio Output
	_audioOutOK = false;
	_audioOutBitRate = 0;
	_audioOutSampleRate = 0;
	_audioOutChannels = 0;
		
	// Audio Input
	_audioInOK = false;;
	_audioInSampleRate = 0;
	_audioInSamples = 0;
	_audioInChannels = 0;
	_audioInBufferL = NULL;
	_audioInBufferR = NULL;
	_audioInSecs = 0;

	// Video Encoding
	// Still empty ;-)
}

AVStream *mObjectEncoder::AddAudioStream (int codecID)
{
    AVCodecContext *codecCtx;
    AVStream *stream;

    stream = av_new_stream (_formatCtx, 1);
    if (!stream)
	{
        fprintf (stderr, "Could not alloc stream\n");
        exit (1);
    }

    codecCtx = stream->codec;
    codecCtx->codec_id   = (CodecID)codecID;
    codecCtx->codec_type = CODEC_TYPE_AUDIO;

	if (_audioOutOK == true)
	{
		codecCtx->bit_rate    = _audioOutBitRate;
		codecCtx->sample_rate = _audioOutSampleRate;
		codecCtx->channels    = _audioOutChannels;
	}
    return stream;
}


void mObjectEncoder::OpenAudio (AVStream *stream)
{
    AVCodecContext *c;
    AVCodec *codec;

    c = stream->codec;

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

    _audioOutSize = 10000;
    _audioOutBuffer = (uint8_t *)malloc (_audioOutSize);

    /* ugly hack for PCM codecs (will be removed ASAP with new PCM
       support to compute the input frame size in samples */
    if (c->frame_size <= 1) {
        _audioInSize = _audioOutSize / c->channels;
        switch(stream->codec->codec_id) {
        case CODEC_ID_PCM_S16LE:
        case CODEC_ID_PCM_S16BE:
        case CODEC_ID_PCM_U16LE:
        case CODEC_ID_PCM_U16BE:
            _audioInSize >>= 1;
            break;
        default:
            break;
        }
    } else {
        _audioInSize = c->frame_size;
    }
    _audioOutBufferLR = (int16_t *)malloc (_audioInSize * 2 * c->channels);
}

inline void mObjectEncoder::GetAudioFrame (int frame_size)
{
	static int sw = 0;

	if (_audioInChannels == 2)
	{
		int swn = 0;
		for (int s = 0; s < frame_size; s++)
		{
			if (sw + s >= _audioInSamples)
				break;

			_audioOutBufferLR [2*s + 0] = _audioInBufferL [sw + s];
			_audioOutBufferLR [2*s + 1] = _audioInBufferR [sw + s];
			swn++;
		}
		sw += swn;
	}
	else if (_audioInChannels == 1)
		cout << "error" << endl;
}


inline void mObjectEncoder::WriteAudioFrame (AVStream *stream)
{
    AVCodecContext *c;
    AVPacket pkt;
    av_init_packet(&pkt);

    c = stream->codec;
	memset (_audioOutBufferLR, 0, 2 * _audioInSize * c->channels);
    GetAudioFrame (_audioInSize);

    pkt.size = avcodec_encode_audio (c, _audioOutBuffer, _audioOutSize, _audioOutBufferLR);

    pkt.pts = av_rescale_q (c->coded_frame->pts, c->time_base, stream->time_base);
    pkt.flags |= PKT_FLAG_KEY;
    pkt.stream_index = stream->index;
    pkt.data = _audioOutBuffer;

    if (av_write_frame (_formatCtx, &pkt) != 0)
	{
        fprintf (stderr, "Error while writing audio frame\n");
        exit (1);
    }
}


void mObjectEncoder::CloseAudio(AVStream *stream)
{
    avcodec_close (stream->codec);

    av_free (_audioOutBufferLR);
    av_free (_audioOutBuffer);
}


AVStream *mObjectEncoder::AddVideoStream(AVFormatContext *_formatCtx, int codecID)
{
    AVCodecContext *codecCtx;
    AVStream *stream;

    stream = av_new_stream (_formatCtx, 0);
    if (!stream)
	{
        fprintf (stderr, "Could not alloc stream\n");
        exit (1);
    }

    codecCtx = stream->codec;
    codecCtx->codec_id = (CodecID)codecID;
    codecCtx->codec_type = CODEC_TYPE_VIDEO;

    /* put sample parameters */
    codecCtx->bit_rate = 400000;
    /* resolution must be a multiple of two */
    codecCtx->width = 800;
    codecCtx->height = 600;
    /* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
    codecCtx->time_base.den = STREAM_FRAME_RATE;
    codecCtx->time_base.num = 1;
    codecCtx->gop_size = 12; /* emit one intra frame every twelve frames at most */
    codecCtx->pix_fmt = STREAM_PIX_FMT;
    if (codecCtx->codec_id == CODEC_ID_MPEG2VIDEO)
	{
        /* just for testing, we also add B frames */
        codecCtx->max_b_frames = 2;
    }
    if (codecCtx->codec_id == CODEC_ID_MPEG1VIDEO)
	{
        /* needed to avoid using macroblocks in which some coeffs overflow
           this doesnt happen with normal video, it just happens here as the
           motion of the chroma plane doesnt match the luma plane */
        codecCtx->mb_decision=2;
    }
    // some formats want stream headers to be seperate
    if(!strcmp(_formatCtx->oformat->name, "mp4") || !strcmp(_formatCtx->oformat->name, "mov") || !strcmp(_formatCtx->oformat->name, "3gp"))
        codecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;

    return stream;
}

AVFrame *mObjectEncoder::AllocPicture (int pix_fmt, int width, int height)
{
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = avcodec_alloc_frame();
    if (!picture)
        return NULL;
    size = avpicture_get_size(pix_fmt, width, height);
    picture_buf = (uint8_t *)malloc(size);
    if (!picture_buf) {
        av_free(picture);
        return NULL;
    }
    avpicture_fill((AVPicture *)picture, picture_buf,
                   pix_fmt, width, height);
    return picture;
}

void mObjectEncoder::OpenVideo (AVFormatContext *_formatCtx, AVStream *stream)
{
    AVCodec *codec;
    AVCodecContext *codecCtx;

    codecCtx = stream->codec;

    /* find the video encoder */
    codec = avcodec_find_encoder(codecCtx->codec_id);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        exit(1);
    }

    /* open the codec */
    if (avcodec_open(codecCtx, codec) < 0) {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }

    video_outbuf = NULL;
    if (!(_formatCtx->oformat->flags & AVFMT_RAWPICTURE)) {
        /* allocate output buffer */
        /* XXX: API change will be done */
        video_outbuf_size = 200000;
        video_outbuf = (uint8_t *)malloc(video_outbuf_size);
    }

    /* allocate the encoded raw picture */
    picture = AllocPicture(codecCtx->pix_fmt, codecCtx->width, codecCtx->height);
    if (!picture) {
        fprintf(stderr, "Could not allocate picture\n");
        exit(1);
    }

    /* if the output format is not YUV420P, then a temporary YUV420P
       picture is needed too. It is then converted to the required
       output format */
    tmp_picture = NULL;
    if (codecCtx->pix_fmt != PIX_FMT_YUV420P) {
        tmp_picture = AllocPicture(PIX_FMT_YUV420P, codecCtx->width, codecCtx->height);
        if (!tmp_picture) {
            fprintf(stderr, "Could not allocate temporary picture\n");
            exit(1);
        }
    }
}

/* prepare a dummy image */
void mObjectEncoder::FillYUVImage (AVFrame *pict, int frame_index, int width, int height)
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

void mObjectEncoder::WriteVideoFrame (AVFormatContext *_formatCtx, AVStream *stream)
{
    int out_size, ret;
    AVCodecContext *codecCtx;

    codecCtx = stream->codec;

    if (frame_count >= STREAM_NB_FRAMES) {
        /* no more frame to compress. The codec has a latency of a few
           frames if using B frames, so we get the last frames by
           passing the same picture again */
    } else {
        if (codecCtx->pix_fmt != PIX_FMT_YUV420P) {
            /* as we only generate a YUV420P picture, we must convert it
               to the codec pixel format if needed */
            FillYUVImage(tmp_picture, frame_count, codecCtx->width, codecCtx->height);
            /*
			 * DEPRECATED
			img_convert((AVPicture *)picture, codecCtx->pix_fmt,
                        (AVPicture *)tmp_picture, PIX_FMT_YUV420P,
                        codecCtx->width, codecCtx->height);
						*/
        } else {
            FillYUVImage(picture, frame_count, codecCtx->width, codecCtx->height);
        }
    }


    if (_formatCtx->oformat->flags & AVFMT_RAWPICTURE) {
        /* raw video case. The API will change slightly in the near
           futur for that */
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags |= PKT_FLAG_KEY;
        pkt.stream_index= stream->index;
        pkt.data= (uint8_t *)picture;
        pkt.size= sizeof(AVPicture);

        ret = av_write_frame(_formatCtx, &pkt);
    } else {
        /* encode the image */
        out_size = avcodec_encode_video(codecCtx, video_outbuf, video_outbuf_size, picture);
        /* if zero size, it means the image was buffered */
        if (out_size > 0)
		{
            AVPacket pkt;
            av_init_packet(&pkt);

            pkt.pts = av_rescale_q(codecCtx->coded_frame->pts, codecCtx->time_base, stream->time_base);
            if(codecCtx->coded_frame->key_frame)
                pkt.flags |= PKT_FLAG_KEY;
            pkt.stream_index = stream->index;
            pkt.data = video_outbuf;
            pkt.size = out_size;

            /* write the compressed frame in the media file */
            ret = av_write_frame(_formatCtx, &pkt);
        }
		else
            ret = 0;
    }
    if (ret != 0)
	{
        fprintf (stderr, "Error while writing video frame\n");
        exit (1);
    }
    frame_count++;
}

void mObjectEncoder::CloseVideo (AVFormatContext *_formatCtx, AVStream *stream)
{
    avcodec_close (stream->codec);
    av_free (picture->data[0]);
    av_free (picture);
    if (tmp_picture)
	{
        av_free (tmp_picture->data[0]);
        av_free (tmp_picture);
    }
    av_free (video_outbuf);
}


void mObjectEncoder::SetupAudioIn (int16_t *bufferL, int16_t *bufferR, int samples, int sampleRate, int channels)
{
	/*
	PrintName ();
	printf ("Setting up Audio Input:\n");
	*/
	if (bufferL != NULL && bufferR != NULL && samples > 0 && sampleRate > 0 && channels > 0)
	{
		_audioInBufferL = bufferL;
		_audioInBufferR = bufferR;
		_audioInSampleRate = sampleRate;
		_audioInSamples = samples;
		_audioInChannels = channels;

	}
	_audioInOK = true;
	_audioInSecs = _audioInSamples / (double)_audioInSampleRate;

	/*
	printf ("  BufferL:     %p\n",		_audioInBufferL);
	printf ("  BufferR:     %p\n", 		_audioInBufferR);
	printf ("  Sample Rate: %d Hz\n",	_audioInSampleRate);
	printf ("  Samples:     %d\n", 		_audioInSamples);
	printf ("  Channels:    %d\n",		_audioInChannels);
	printf ("  Duration:    %.4f s\n", 	_audioInSecs);
	*/
}

    
void mObjectEncoder::SetupAudioOut (int audioBitRate, int audioSampleRate, int audioChannels)
{
	/*
	PrintName ();
	printf ("Setting up Audio Output:\n");
	*/
	if (audioBitRate > 0 && audioSampleRate > 0 && audioChannels > 0)
	{
		_audioOutBitRate    = audioBitRate;
		_audioOutSampleRate = audioSampleRate;
		_audioOutChannels   = audioChannels;
	}
	_audioOutOK = true;

	/*
	printf ("  Bit Rate:    %d bps\n", _audioOutBitRate);
	printf ("  Sample Rate: %d Hz\n",  _audioOutSampleRate);
	printf ("  Channels:    %d\n",     _audioOutChannels);
	*/
}


void mObjectEncoder::ShowAudioProgress (double nowPTS, double totPTS)
{
	fprintf (stderr, "  Encoding at time: %.4f/%.4f s\r", nowPTS, totPTS);
	fflush (stderr);
}


bool mObjectEncoder::EncodeAudio (char *filename)
{
    AVOutputFormat *formatOut;
    AVStream *streamAudio;
    AVStream *streamVideo;
    
	double audioPTS;
    double videoPTS;
    
	if (!_audioOutOK || !_audioInOK)
	{
		fprintf (stderr, "Missing Audio Input and Audio Output settings\n");
		return false;
	}
   
	//double frequency = 48000;
	//_audioOutSecs = (double)(samples / frequency);
    //cout << _audioOutSecs << endl;
	//char *filename = "Data/audio.wav";
	av_register_all();
    formatOut = guess_format (NULL, filename, NULL);
    
	if (!formatOut) 
	{
        printf ("Could not deduce output format from file extension: using MPEG.\n");
        formatOut = guess_format ("mpeg", NULL, NULL);
    }
    if (!formatOut)
	{
        fprintf (stderr, "Could not find suitable output format\n");
        exit (1);
    }
    
	_formatCtx = av_alloc_format_context ();
    if (!_formatCtx)
	{
        fprintf (stderr, "Memory error\n");
        exit (1);
    }
    _formatCtx->oformat = formatOut;
    snprintf (_formatCtx->filename, sizeof(_formatCtx->filename), "%s", filename);
   
	/* add the audio and video streams using the default format codecs
       and initialize the codecs */
    streamVideo = NULL;
    streamAudio = NULL;

	/*
	PrintName ();
	printf ("Setting up Output Format:\n");
	*/
    if (formatOut->video_codec != CODEC_ID_NONE) 
	{
        streamVideo = AddVideoStream (_formatCtx, formatOut->video_codec);
		//printf ("  Video: supported\n");
    }
	else
		//printf ("  Video: not supported\n");

    if (formatOut->audio_codec != CODEC_ID_NONE)
	{
        streamAudio = AddAudioStream (formatOut->audio_codec);
		//printf ("  Audio: supported\n");
    }
	else
		//printf ("  Audio: not supported\n");
    
	if (av_set_parameters (_formatCtx, NULL) < 0)
	{
        fprintf (stderr, "Invalid output format parameters\n");
        exit (1);
    }

	if (_formatCtx == NULL)
	{
        fprintf (stderr, "Something went wrong with format\n");
        exit (1);
	}

#ifdef VERBOSE
	dump_format (_formatCtx, 0, filename, 1);
#endif
    
	if (streamVideo)
        OpenVideo (_formatCtx, streamVideo);
    if (streamAudio)
        OpenAudio (streamAudio);

    if (!(formatOut->flags & AVFMT_NOFILE))
	{
        if (url_fopen (&_formatCtx->pb, filename, URL_WRONLY) < 0)
		{
            fprintf (stderr, "Could not open '%s'\n", filename);
            exit (1);
        }
    }

    av_write_header (_formatCtx);

	PrintName ();
	printf ("Encoding Audio:\n");
	while (true)
	{
        if (streamAudio)
            audioPTS = (double)streamAudio->pts.val * streamAudio->time_base.num / streamAudio->time_base.den;
        else
            audioPTS = (double)0.0;

        if (streamVideo)
            videoPTS = (double)streamVideo->pts.val * streamVideo->time_base.num / streamVideo->time_base.den;
        else
            videoPTS = (double)0.0;
		
		ShowAudioProgress (audioPTS, _audioInSecs);

        if ((!streamAudio || (double)audioPTS >= (double)_audioInSecs) &&
            (!streamVideo || (double)videoPTS >= (double)_audioInSecs))
            break;
		
        if (!streamVideo || (streamVideo && streamAudio && audioPTS < videoPTS)) 
            WriteAudioFrame (streamAudio);
		else 
            WriteVideoFrame (_formatCtx, streamVideo);
	
    }
	cout << endl << flush;

    if (streamVideo)
        CloseVideo (_formatCtx, streamVideo);
    if (streamAudio)
        CloseAudio (streamAudio);

    av_write_trailer (_formatCtx);

    for (unsigned int i = 0; i < _formatCtx->nb_streams; i++)
	{
        av_freep (&_formatCtx->streams[i]->codec);
        av_freep (&_formatCtx->streams[i]);
    }

    if (!(formatOut->flags & AVFMT_NOFILE))
        url_fclose (&_formatCtx->pb);

    av_free (_formatCtx);

	return true;
}

/*
void mObjectEncoder::Main (void)
{
	const char *filename;
    AVOutputFormat *formatOut;
    AVFormatContext *_formatCtx;
    AVStream *streamAudio;
    AVStream *streamVideo;
    double audioPTS;
    double videoPTS;
    int i;

    // initialize libavcodec, and register all codecs and formats
    av_register_all();

    filename = "Data/audio.mp3";

    // auto detect the output format from the name. default is mpeg.
    formatOut = guess_format (NULL, filename, NULL);
    if (!formatOut) 
	{
        printf ("Could not deduce output format from file extension: using MPEG.\n");
        formatOut = guess_format ("mpeg", NULL, NULL);
    }
    if (!formatOut)
	{
        fprintf (stderr, "Could not find suitable output format\n");
        exit (1);
    }

    // allocate the output media context
    _formatCtx = av_alloc_format_context ();
    if (!_formatCtx)
	{
        fprintf (stderr, "Memory error\n");
        exit (1);
    }
    _formatCtx->oformat = formatOut;
    snprintf (_formatCtx->filename, sizeof(_formatCtx->filename), "%s", filename);

    // add the audio and video streams using the default format codecs
    //   and initialize the codecs
    streamVideo = NULL;
    streamAudio = NULL;
    if (formatOut->video_codec != CODEC_ID_NONE) 
	{
        streamVideo = AddVideoStream (_formatCtx, formatOut->video_codec);
    }
	else
		printf ("Format does not support Video\n");

    if (formatOut->audio_codec != CODEC_ID_NONE)
	{
        streamAudio = AddAudioStream (_formatCtx, formatOut->audio_codec);
    }
	else
		printf ("Format does not support Audio\n");

    // set the output parameters (must be done even if no
    // parameters).
    if (av_set_parameters(_formatCtx, NULL) < 0)
	{
        fprintf(stderr, "Invalid output format parameters\n");
        exit(1);
    }
	printf ("Format dumped\n");

    dump_format (_formatCtx, 0, filename, 1);

    // now that all the parameters are set, we can open the audio and
    //   video codecs and allocate the necessary encode buffers 
    if (streamVideo)
        OpenVideo (_formatCtx, streamVideo);
    if (streamAudio)
        OpenAudio (_formatCtx, streamAudio);

    // open the output file, if needed 
    if (!(formatOut->flags & AVFMT_NOFILE))
	{
        if (url_fopen(&_formatCtx->pb, filename, URL_WRONLY) < 0)
		{
            fprintf(stderr, "Could not open '%s'\n", filename);
            exit(1);
        }
    }

    // write the stream header, if any
    av_write_header(_formatCtx);

    for(;;) {
        // compute current audio and video time
        if (streamAudio)
            audioPTS = (double)streamAudio->pts.val * streamAudio->time_base.num / streamAudio->time_base.den;
        else
            audioPTS = 0.0;

        if (streamVideo)
            videoPTS = (double)streamVideo->pts.val * streamVideo->time_base.num / streamVideo->time_base.den;
        else
            videoPTS = 0.0;

        if ((!streamAudio || audioPTS >= _audioOutSecs) &&
            (!streamVideo || videoPTS >= _audioOutSecs))
            break;

        // write interleaved audio and video frames 
        if (!streamVideo || (streamVideo && streamAudio && audioPTS < videoPTS)) 
            WriteAudioFrame (_formatCtx, streamAudio);
		else 
            WriteVideoFrame (_formatCtx, streamVideo);
    }

    // close each codec
    if (streamVideo)
        CloseVideo(_formatCtx, streamVideo);
    if (streamAudio)
        CloseAudio(_formatCtx, streamAudio);

    // write the trailer, if any
    av_write_trailer (_formatCtx);

    // free the streams
    for(i = 0; i < _formatCtx->nb_streams; i++)
	{
        av_freep (&_formatCtx->streams[i]->codec);
        av_freep (&_formatCtx->streams[i]);
    }

	// close the output file
    if (!(formatOut->flags & AVFMT_NOFILE))
        url_fclose (&_formatCtx->pb);

    // free the stream
    av_free (_formatCtx);
}
*/


/*
void mObjectEncoder::WriteAudioFrame (AVFormatContext *_formatCtx, AVStream *st)
{
	cout << "MERDA!" << endl;
    AVCodecContext *c;
    AVPacket pkt;
    av_init_packet(&pkt);

    c = st->codec;

    GetAudioFrame (samples, _audioInSize, c->channels);

    pkt.size= avcodec_encode_audio (c, _audioOutBuffer, _audioOutSize, samples);

    pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
    pkt.flags |= PKT_FLAG_KEY;
    pkt.stream_index= st->index;
    pkt.data= _audioOutBuffer;

    // write the compressed frame in the media file
    if (av_write_frame(_formatCtx, &pkt) != 0) {
        fprintf(stderr, "Error while writing audio frame\n");
        exit(1);
    }
}
*/


/*
void mObjectEncoder::GetAudioFrame (int16_t *samples, int frame_size, int nb_channels)
{
    int j, i, v;
    int16_t *q;
	cout << "frame size: " << frame_size << endl;

    q = samples;
    for(j=0;j<frame_size;j++)
	{
        v = (int)(sin(t) * 10000);
        for(i = 0; i < nb_channels; i++)
            *q++ = v;
        t += tincr;
        tincr += tincr2;
    }
}
*/
#endif
