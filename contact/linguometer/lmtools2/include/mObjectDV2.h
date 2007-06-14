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

#ifndef MOBJECTDV2_H
#define MOBJECTDV2_H


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
#include "mObjectIMG.h"

#define MOBJECTDV2_FNSIZE	1024
#define MOBJECTDV2_ASSIZE	500*AVCODEC_MAX_AUDIO_FRAME_SIZE


class mObjectDV2 : public mClass {
	public:
	mObjectDV2();
	//~mObjectDV2();

	bool setFile(char *filename = NULL);
    bool init(void);

	bool allocBufferPCM (int16_t **buffer, int &bytes);
	bool allocBufferRGB (int8_t **buffer, int &bytes);

	bool readFrame (AVPacket *packet);
	bool getAudioFrame(AVPacket *packet, int16_t **buffer, int &bytes);
	bool getVideoFrame(AVPacket *packet, int8_t **buffer, int &bytes);

	bool hasVideo(void);
	bool hasAudio(void);

	int getVideoID(void);
	int getAudioID(void);
	
	int getWidth (void);
	int getHeight (void);
	double getFrameRate (void);
	int getPixelFormat (void);

	int getChannels (void);
	int getSampleRate (void);
	int getSampleFormat (void);

	/* This method is buggy.
	 * Maybe I'll drop it.
	 */
	int64_t getPTS (AVPacket *packet);
	int64_t getPTS (AVPacket *packet, int id);
	int64_t getVideoPTS (AVPacket *packet);
	int64_t getAudioPTS (AVPacket *packet);
		
	bool seek(int64_t pts);
	bool showProgress (int64_t pts);
	
	private:
    bool registerCodecs(void);
	bool searchStreams(void);
	bool searchCodecs(void);
	bool openCodecs(void);
	bool allocBuffers(void);

		
	char *_filename;
    AVFormatContext *_formatCtx;
	AVCodecContext *_codecCtxVideo;
	AVCodecContext *_codecCtxAudio;
	AVCodec *_codecVideo;
	AVCodec *_codecAudio;
	AVFrame *_frameYUV; 
	AVFrame *_frameRGB;
    bool _streamVideo;
    bool _streamAudio;
    int _streamVideoID;
    int _streamAudioID;
};


mObjectDV2::mObjectDV2(void) {
	SetName("mObjectDV");
	
	//PrintName();
	//printf("Starting %s\n", GetName());
	
	_filename = NULL;

	_streamVideo = -1;
	_streamAudio = -1;
}


bool mObjectDV2::setFile(char *filename) {
	if (filename == NULL || filename == EMPTY)
		return false;
	
	_filename = new char [FN_SIZE];
	memset (_filename, 0, FN_SIZE);
	strcpy (_filename, filename);
	return true;
}


bool mObjectDV2::allocBufferPCM (int16_t **buffer, int &bytes) {
	bytes = AVCODEC_MAX_AUDIO_FRAME_SIZE * sizeof(int16_t);
	*buffer = (int16_t*)malloc(bytes);
	memset (*buffer, 0, bytes);
	return true;
}


bool mObjectDV2::allocBufferRGB (int8_t **buffer, int &bytes) {
	bytes = avpicture_get_size(PIX_FMT_RGB24, 
			_codecCtxVideo->width, 
			_codecCtxVideo->height);
	*buffer = (int8_t*)malloc(bytes);
	memset (*buffer, 0, bytes);
	
	return true;
}


bool mObjectDV2::readFrame (AVPacket *packet) {
	if (av_read_frame(_formatCtx, packet) == 0)
		return true;

	return false;
}


bool mObjectDV2::getAudioFrame(AVPacket *packet, int16_t **buffer, int &bytes) {
	if (!_streamAudio)
		return false;

	if (packet->stream_index != _streamAudioID)
		return false;

	/* Decode audio frames using:
	 *  avcodec_decode_audio2()
	 * FFMPEG version (gentoo x86): 
	 *  media-video/ffmpeg-0.4.9_p20061016
	 * Description:
	 *  empty 
	 */
	/* 
	int bytes_expected = 0;
	bytes = avcodec_decode_audio(_codecCtxAudio, 
			*buffer,
			&bytes_expected, 
			packet->data, 
			packet->size);
	*/
	
	/* Decode audio frames using:
	 *  avcodec_decode_audio2()
	 * FFMPEG version (gentoo x86): 
	 *  media-video/ffmpeg-0.4.9_p20070129
	 * Description:
	 *  avcodec_decode_audio2() replaces the old
	 *  avcodec_decode_audio() function (now deprecated).
	 */
	int bytes_expected = AVCODEC_MAX_AUDIO_FRAME_SIZE;
	bytes = avcodec_decode_audio2(_codecCtxAudio, 
			*buffer,
			&bytes_expected, 
			packet->data, 
			packet->size);

	if (bytes == bytes_expected)
		return true;
	else
		bytes = -1;
	return false;
}


bool mObjectDV2::getVideoFrame(AVPacket *packet, int8_t **buffer, int &bytes) {
	if (!_streamVideo)
		return false;

	if (packet->stream_index != _streamVideoID)
		return false;

	int bytes_decoded = 0;
	
	bytes = avcodec_decode_video(_codecCtxVideo, 
			_frameYUV, 
			&bytes_decoded,
			packet->data, 
			packet->size);

	if (bytes <= 0 || bytes_decoded <= 0)
		return false;

	img_convert((AVPicture *)_frameRGB, 
			PIX_FMT_RGB24, 
			(AVPicture *)_frameYUV, 
			_codecCtxVideo->pix_fmt, 
			_codecCtxVideo->width, 
			_codecCtxVideo->height);
	bytes = _codecCtxVideo->width * _codecCtxVideo->height * 3;

	mempcpy(*buffer, _frameRGB->data[0], bytes);
	return true;
}


//-------------------------------------//
bool mObjectDV2::init(void) {
    bool result=registerCodecs();
    result &= searchStreams();
    result &= searchCodecs();
    result &= openCodecs();
    result &= allocBuffers();

#ifdef VERBOSE
	dump_format (_formatCtx, 0, _filename, false);
#endif

    return result;
}


bool mObjectDV2::registerCodecs(void) {
	av_register_all();
	if (av_open_input_file(&_formatCtx,_filename,NULL,0,NULL) != 0) 
		return false;
    return true;
}


bool mObjectDV2::searchStreams(void) {
	if (av_find_stream_info(_formatCtx)<0)
		return false;
	
	_streamAudio = false;
	_streamVideo = false;
	_streamAudioID = -1;
	_streamVideoID = -1;
	for (unsigned int i=0; i<_formatCtx->nb_streams; i++) {
		if (_formatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO)
			_streamVideoID = i;
		if (_formatCtx->streams[i]->codec->codec_type==CODEC_TYPE_AUDIO)
			_streamAudioID = i;
		if (_streamAudioID!=-1 && _streamVideoID!=-1)
			break;
	}
	if (_streamAudioID != -1)
		_streamAudio = true;
	if (_streamVideoID != -1)
		_streamVideo = true;
	return true;
}


bool mObjectDV2::searchCodecs(void) {
	/* Bug!
	 *
	if (!_streamAudio && !_streamVideo)
		return false;
	*/

	if (_streamVideo) {
		_codecCtxVideo = _formatCtx->streams[_streamVideoID]->codec;
		_codecVideo = avcodec_find_decoder(_codecCtxVideo->codec_id);
	}
	if (_streamAudio) {
		_codecCtxAudio = _formatCtx->streams[_streamAudioID]->codec;
		_codecAudio = avcodec_find_decoder(_codecCtxAudio->codec_id);
	}
    return true;
}


bool mObjectDV2::openCodecs(void) {
	if (_streamAudio) 
		if (avcodec_open(_codecCtxAudio, _codecAudio) < 0)
			return false;
	
	if (_streamVideo) 
		if (avcodec_open(_codecCtxVideo, _codecVideo) < 0)
			return false;
	
    return true;
}


bool mObjectDV2::allocBuffers(void) {
	if (_streamVideo) {
		_frameYUV = avcodec_alloc_frame();
		_frameRGB = avcodec_alloc_frame();
		avpicture_alloc((AVPicture*)_frameRGB, 
				PIX_FMT_RGB24, 
				_codecCtxVideo->width, 
				_codecCtxVideo->height);
		
		/*
		   int bytes = avpicture_get_size(PIX_FMT_RGB24, 
		   _codecCtxVideo->width, 
		   _codecCtxVideo->height);
		 */
	}
    return true;
}

bool mObjectDV2::hasVideo(void) {
	/* Bug: 
	 * 	was > 0
	 * 	is > -1
	 */
	return _streamVideo > -1 ? true : false;
};

bool mObjectDV2::hasAudio(void) {
	/* Bug: 
	 * 	was > 0
	 * 	is > -1
	 */
	return _streamAudio > -1 ? true : false; 
};

int mObjectDV2::getVideoID(void) {
	return hasVideo() ? _streamVideoID : -1; 
};

int mObjectDV2::getAudioID(void) {
	return hasAudio() ? _streamAudioID : -1; 
};

int mObjectDV2::getWidth(void) {
	return hasVideo() ? _codecCtxVideo->width : -1;
}

int mObjectDV2::getHeight(void) {
	return hasVideo() ? _codecCtxVideo->height : -1;
}

double mObjectDV2::getFrameRate(void) {
	return hasVideo() ? 
		av_q2d (_formatCtx->streams[getVideoID()]->r_frame_rate) : -1;
}

int mObjectDV2::getPixelFormat(void) {
	/* I always work with RGB 24bit 
	 * --> This method returns the 
	 *     format in bytes 
	 */
	return hasVideo() ? 3 : -1;
}

int mObjectDV2::getSampleRate(void) {
	return hasAudio() ? _codecCtxAudio->sample_rate : -1;
}

int mObjectDV2::getSampleFormat(void) {
	/* I always work with PCM 16bit signed
	 * --> This method returns the 
	 *     format in bytes 
	 * If needed, enum:
	 *  1	8bit, signed
	 *  2	16bit, signed
	 *  3	24bit, signed (does it exist?)
	 *  4	32bit, signed (does it exist?)
	 *  10	8bit,  unsigned
	 *  20	16bit, unsigned
	 *  30	24bit, unsigned (does it exist?)
	 *  40	32bit, unsigned (does it exist?)
	 */
	return hasAudio() ?  2 : -1;
}

int mObjectDV2::getChannels(void) {
	return hasAudio() ? _codecCtxAudio->channels : -1;
}

int64_t mObjectDV2::getPTS(AVPacket *packet) {
	int id = 0;
	
	if (hasVideo())
		id = getVideoID();
	else if(hasAudio())
		id = getAudioID();
	else
		return -1;

	return getPTS(packet, id);	
}

int64_t mObjectDV2::getPTS(AVPacket *packet, int id) {
	if (id == _streamAudioID || id == _streamVideoID)
		return av_rescale_q (packet->pts, 
				_formatCtx->streams[id]->time_base, 
				AV_TIME_BASE_Q);

	return -1;
}


int64_t mObjectDV2::getVideoPTS(AVPacket *packet) { 
	return getPTS(packet, getVideoID()); 
};


int64_t mObjectDV2::getAudioPTS(AVPacket *packet) { 
	return getPTS(packet, getAudioID()); 
};
	

bool mObjectDV2::showProgress(int64_t pts) {
	int hours = 0, mins = 0, 
			   secs = 0, us = 0;

	if (!mObjectDV2Tools::pts2time(pts, hours, mins, secs, us))
		return false;

	fprintf(stderr, "Decoding at time: %02d:%02d:%02d.%01d  \r",
		hours, mins, secs, (10 * us) / AV_TIME_BASE);
	fflush(stderr);
	return true;
}


bool mObjectDV2::seek(int64_t pts)
{
	int id = -1;
	if (hasVideo())
		id = getVideoID();
	else if (!hasVideo() && hasAudio())
		id = getAudioID();
	else
		return false;

	if (av_seek_frame (_formatCtx, 
			id,
			av_rescale_q (pts, 
				AV_TIME_BASE_Q, 
				_formatCtx->streams[id]->time_base),
			AVSEEK_FLAG_BACKWARD) >= 0)
		return true;

	return false;
}


int mDV2PCM (char* filename, int64_t pts0, int64_t pts1, mObjectPCM<int16_t> *pcm) {
	/* Let's create a new decoder */
	mObjectDV2 *stream = new mObjectDV2();
	
	/* It's time to pass the file we
	 * wish to work with to the decoder
	 */
	if (!stream->setFile(filename))
		return -1;
	
	/* This method bootstraps the decoder itself */
	if (!stream->init())
		return -1;

	/* We are working with a DV file.
	 * This means that we need to handle
	 * PCM-Audio and RGB-Video frames,
	 * creating proper structures.
	 */
	/* Let's check if the stream is A+V */
	if (!stream->hasAudio() || !stream->hasVideo())
		return -1;

	/* Let's create a custom PCM stream */
	///////int channels = stream->getChannels();
	int sample_format = stream->getSampleFormat();

	if (sample_format != 2) 
		return -1;

	///////mObjectPCM<int16_t> *pcm = NULL;
	///////pcm = new mObjectPCM<int16_t>(channels, MPCM_STREAM_BIG);

	/* Let's create a custom RGB frame */
	int height = stream->getHeight();
	int width = stream->getWidth();
	int pixel_format = stream->getPixelFormat();
	if (pixel_format != 3) 
		return -1;

	mObjectIMG *rgb = NULL;
	rgb = new mObjectIMG;
	rgb->New(width, height, MIMAGE_RGB);
	
	/* Perfect! Now we have a PCM stream
	 * and an RGB frame.
	 * --> Real Video Frames processing could 
	 *     be done using STL containers.
	 * Let's create what's needed for decoding.
	 */
	/* The decoder stores the frames read 
	 * in a "packet" of data.
	 */
	AVPacket *packet = new AVPacket;

	/* We'll ask the decoder to store
	 * the decoded data in our buffers 
	 */
	int16_t *buffer_PCM = NULL;
	int8_t *buffer_RGB = NULL;

	/* In order to check if everything
	 * is ok, we need to check a couple of return
	 * variables (better explained later on).
	 */
	int bytes_PCM = 0,		/* PCM Buffer Size in Bytes */
		bytes_RGB = 0;		/* RGB Buffer Size in Bytes */
	int bytes_PCMr = 0, 	/* PCM Read Bytes */
		bytes_RGBr = 0; 	/* PCM Read Bytes */
	
	/* Here we have a corretion factor.
	 * This is a work-around since mObjectPCM handles
	 * PCM streams in terms of samples, not of bytes 
	 * and/or seconds (hours, PTS etc).
	 */
	int cf_PCM = sizeof(int16_t);

	/* Let's ask the decoder to allocate the 
	 * buffers for us. 
	 * The decoder also fills the bytes_* variables,
	 * so we know how big the buffer is.
	 */
	stream->allocBufferPCM (&buffer_PCM, bytes_PCM);
	stream->allocBufferRGB (&buffer_RGB, bytes_RGB);

	/* Here we go! Main loop!
	 * Pseudocode:
	 * while(read next frame, put the data in packet)
	 *  if(the packet contains an audio {frame copied to buffer_PCM})
	 *     if(the frame has been decoded correctly)
	 *       add the decoded samples to the PCM stream;
	 *  if(the packet contains a video {frame copied to buffer_RGB})
	 *     if(the frame has been decoded correctly)
	 *       add the decoded pixel to the RGB image;
	 *       display decoding progress;
	 */
	int64_t pts = 0;
	if (pts0 > 0)
		stream->seek(pts0);
	while (stream->readFrame(packet)) {
		if (stream->getAudioFrame(packet, &buffer_PCM, bytes_PCMr)) 
			if (bytes_PCMr)
				pcm->AddSamplesLR(buffer_PCM, bytes_PCMr/cf_PCM);
		if (stream->getVideoFrame(packet, &buffer_RGB, bytes_RGBr)) 
			if (bytes_RGBr) {
				//rgb->DumpBuffer ((unsigned char*)buffer_RGB);
				stream->showProgress (stream->getVideoPTS(packet));
			
				// Develop //
				pts = stream->getPTS(packet);
					if(pts >= pts1 && pts1 > pts0)
						break;
			}
	}
	delete stream;


	return 0;

}

int mAudio2PCM (char* filename, int64_t pts0, int64_t pts1, mObjectPCM<int16_t> *pcm) {
	mObjectDV2 *stream = new mObjectDV2();
	
	if (!stream->setFile(filename))
		return -1;
	if (!stream->init())
		return -1;
	if (!stream->hasAudio())
		return -1;
	
	unsigned int channels = stream->getChannels();
	
	int sample_format = stream->getSampleFormat();
	if (sample_format != 2) 
		return -1;

	AVPacket *packet = new AVPacket;

	int16_t *buffer_PCM = NULL;

	int bytes_PCM = 0;
	int bytes_PCMr = 0;
	
	int cf_PCM = sizeof(int16_t);

	stream->allocBufferPCM (&buffer_PCM, bytes_PCM);

	int64_t pts = 0;
	if (pts0 > 0)
		stream->seek(pts0);
	while (stream->readFrame(packet)) {
		if (stream->getAudioFrame(packet, &buffer_PCM, bytes_PCMr)) 
			if (bytes_PCMr) {
				if (channels == 2)
					pcm->AddSamplesLR(buffer_PCM, bytes_PCMr/cf_PCM);
				else 
					pcm->AddSamplesM(buffer_PCM, bytes_PCMr/cf_PCM);
				stream->showProgress (stream->getAudioPTS(packet));
			}
		pts = stream->getAudioPTS(packet);
		if(pts0 > 0 && pts1 > 0)
			if(pts >= pts1 && pts1 > pts0)
				break;
	}
	cout << endl;
	delete stream;


	return 0;

}


#endif
