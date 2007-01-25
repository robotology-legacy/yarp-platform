/* Coded by M. Tavella
 * email michele@liralab.it
 * web   http://mt.homelinux.org 
 */

#ifndef MOBJECTDV_H
#define MOBJECTDV_H

/*
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <fstream>
using namespace std;
*/

#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>

#include "mInclude.h"
#include "mClass.h"
#include "mObjectPCM.h"
#include "mObjectIMG.h"
#include "mObjectEncoder.h"
#include "mContainer.h"

//#define DEBUG_STREAM
//#define DEBUG
//#define USE_MOBJIMG

//#define USE_MAGICK

#ifdef USE_MAGICK
#	include <deque>
#	include <Magick++.h>
	using namespace Magick;
#endif

#define MOBJECTDV_FNSIZE	1024
#define MOBJECTDV_ASSIZE	500*AVCODEC_MAX_AUDIO_FRAME_SIZE

#ifndef DEBUG
#	ifndef DEBUG_STREAM
#		define PROGRESS
#	endif
#endif

struct mTime 
{
	int h;
	int m;
	int s;
	int us;	
};

class mObjectDV : public mClass
{
	public: 	/* Methods */
		
		/* The contructor takes care of
		 * setting a couple of important flags 
		 * (to enable/diasable Audio and/or 
		 * Video support).
		 * mObjectDV detects if a stream has got 
		 * video and/or audio data.
		 * Those flags should be used for debugging!
		 * Also, it calls Init (), the "real
		 * constructor"
		 */
		mObjectDV (bool EnableVideo = true, bool EnableAudio = true);

		/* Self-Explaining */
		~mObjectDV ();

		/* The constructor calls Init. This
		 * method could be useful once I 
		 * add a Reinit () function.
		 */
		void Init (void);
		
		/* Sets the A/V file. mProcessDV
		 * was written with DV Type 1,
		 * DV Type 2 and DV Raw video
		 * formats in mind.
		 * Also, the audio should be plain
		 * PCM.
		 */
		void SetStream (char *file = NULL);

		/* Sets the Video output folder.
		 * This folder will be used to extract
		 * PCM audio and PPM video.
		 */
		void SetVideoOut (char *folder = NULL);
		
		/* Sets the Audio output file.
		 * This folder will be used to extract
		 * PCM audio.
		 */
		void SetAudioOut (char *folder = NULL);

		/* Since decoding a long DV file could
		 * be painful, you could decide to decode 
		 * and/or export video or audio.
		 */
		void SetupAudio (bool Decode = true, bool Export = false);
		void SetupVideo (bool Decode = true, bool Export = false);

		/* This method should be called 
		 * just before DecodeStart () since
		 * it manages to call the proper private
		 * methods to initialize everything 
		 * the decode loop needs.
		 */
		bool DecodeInit  (void);

		/* PrintDetails is a pretty dumb 
		 * method. It displays useful
		 * informations about the Audio 
		 * and the Video streams
		 */
		void PrintDetails (void);

		/* This method is used to test 
		 * unstable functionalities
		 */
		void Test (void);

		/* Audio Decoding method;
		 * Seeks, Decodes, Stores 
		 * and exports
		 */
		bool DecodeAudio (void);
		
		/* Audio Decoding method;
		 * Seeks, Decodes, Stores 
		 * and exports
		 */
		bool DecodeVideo (void);
	
		/* Decode To and From HH:MM:SS
		 */
		bool DecodeFrom (int h, int m, int s);
		bool DecodeTo   (int h, int m, int s);

		/* Audio/Video seek function.
		 * Called by DecodeAudio () 
		 * and DecodeVideo ()
		 */
		int  Seek (void);
	
		/* Eye candy to display 
		 * decoding progress.
		 */
		void ShowProgress (int64_t timepos, int counter);

		/* Convets a PTS time to HH:MM:SS
		 */
		inline void PTS2Time (int64_t t, int &h, int &m, int &s);
		
		/* Convets an HH:MM:SS time to PTS
		 */
		inline void Time2PTS (int64_t &t, int h, int m, int s);
		
		/* Get stream PTS (PTS time) 	
		 */
		int64_t GetPTS (int streamID);

	private: 	/* Methods */
		/* The next group of methods
		 * is used to setup codecs, streams,
		 * buffers and to collect useful 
		 * infos about the A/V streams.
		 */
		bool RegisterCodecs (void);
		bool SearchStreams  (void);
		bool SearchCodecs   (void);
		bool OpenCodecs     (void);
		bool AllocBuffers   (void);
		bool CollectInfos   (void);
	
		/* Total number of frames
		 * This variable is used to
		 * determine how much space should
		 * be allocated in STL deque 
		 * (if you use Magick++) or in 
		 * mContainer (if you use mObjectIMG)
		 */
		int	_framesVideo;
		int	_framesAudio;

	public:	
		/* Buffers are used to store Packet/Frame
		 * informations while reading streams.
		 * Later on buffers allow decoding the
		 * stream.
		 * Video buffer: YUV 8bits
		 * Audio buffer: PCM 16bits
		 */
		uint8_t *_bufferVideo;
		int16_t *_bufferAudio;


#ifndef USE_MAGICK
		mContainer<mObjectIMG> *streamVideo;
#else
		deque<Image> *streamVideo;
#endif
		//mObjectPCM<signed short> *streamAudio;
		//mObjectPCM<short> *streamAudio;
		mObjectPCM<int16_t> *streamAudio;

	private:
		char			*_file;
		char			*_videoOut;
		char			*_audioOut;
		
		AVFormatContext *_formatCtx;
		
		AVPacket        _packet;

		AVCodecContext  *_codecCtxVideo;
		AVCodecContext  *_codecCtxAudio;

		AVCodec         *_codecVideo;
		AVCodec         *_codecAudio;

		AVFrame         *_frameYUV; 
		AVFrame         *_frameRGB;

		int             _streamVideoID;
		int             _streamAudioID;
		
		int             _frameYUVFinished;
		int				_framePCMLenght;
		int				_framePCMBytes;
		int             _frameYUVBytes;

		bool 			_enableAudio;
		bool 			_enableVideo;

		bool 			_decodeVideo;
		bool 			_decodeAudio;
		bool 			_exportVideo;
		bool 			_exportAudio;

		int64_t			_seekT0;
		int64_t			_seekT1;
#ifdef USE_MAGICK
		Image *_imageProc;
		char  *_imageProcName;
#else
		mObjectIMG *_imageProc;
		char  *_imageProcName;
		bool  _imageProcAlloc;
#endif
};


mObjectDV::mObjectDV (bool EnableVideo, bool EnableAudio)
{
	SetName ("mObjectDV");
	
	PrintName ();
	printf ("Starting %s\n", GetName ());
	
	_enableVideo = EnableVideo;
	_enableAudio = EnableAudio;

	PrintName ();
	printf ("Current Settings:\n");
	printf ("  Video %s, Audio %s\n", 
			_enableVideo ? "Enabled" : "Disabled",
			_enableAudio ? "Enabled" : "Disabled");

	Init ();

}


mObjectDV::~mObjectDV ()
{
	PrintName ();
	printf ("Destroying %s\n", GetName ());

	if (_formatCtx != NULL)
		delete _formatCtx;
	if (_codecCtxVideo != NULL)
		delete _codecCtxVideo;

	if (_codecCtxAudio != NULL)
		delete _codecCtxAudio;

	if (_frameYUV != NULL)
		delete _frameYUV;

	if (_frameRGB != NULL)
		delete _frameRGB;

	if (_bufferVideo != NULL)
		delete _bufferVideo;
    
	if (_bufferAudio != NULL)
		delete _bufferAudio;
	
	if (streamAudio != NULL)
		delete streamAudio;

#ifndef USE_MAGICK
	if (streamVideo != NULL)
		delete streamVideo;
#endif
}


void mObjectDV::Init (void)
{
	PrintName ();
	printf ("Initializing\n");
    _formatCtx = 0;

	_streamVideoID = -1;
    _streamAudioID = -1;
    
	_codecCtxVideo = NULL;
    _codecCtxAudio = NULL;
    
	_codecVideo = NULL;
    _codecAudio = NULL;
    
	_frameYUV = NULL; 
    _frameRGB = NULL;
   
	_frameYUVFinished = 0;
	_framePCMLenght = 0;
	_framePCMBytes = 0;
    _frameYUVBytes = 0;

	_bufferVideo = NULL;
    _bufferAudio = NULL;
	
	_framesVideo = 0;
	_framesAudio = 0;

	_file = new char [MOBJECTDV_FNSIZE];
	memset (_file, 0, MOBJECTDV_FNSIZE);

	if (_enableVideo)
		streamVideo = new mContainer<mObjectIMG>;
		
	_decodeVideo = false;
	_decodeAudio = false;
	_exportVideo = false;
	_exportAudio = false;

	_seekT1 = -1;
	_seekT0 = -1;

	_imageProc = new mObjectIMG;
	_imageProcName = new char [MOBJECTDV_FNSIZE];
}


void mObjectDV::SetStream (char *file)
{
	assert (file != NULL);
	assert (file != "");
	strcpy (_file, file);
	PrintName ();
	printf ("Input stream:  %s\n", _file);
}


void mObjectDV::SetVideoOut (char *folder)
{
	assert (folder != NULL);
	assert (folder != "");

	if (_videoOut != NULL)
		delete _videoOut;

	_videoOut = new char [MOBJECTDV_FNSIZE];
	memset (_videoOut, 0, MOBJECTDV_FNSIZE);

	strcpy (_videoOut, folder);
	PrintName ();
	printf ("Video output folder: %s\n", _videoOut);
	
}


void mObjectDV::SetAudioOut (char *file)
{
	assert (file != NULL);
	assert (file != "");


	if (_audioOut != NULL)
		delete _audioOut;
	
	_audioOut = new char [MOBJECTDV_FNSIZE];
	memset (_audioOut, 0, MOBJECTDV_FNSIZE);
	
	strcpy (_audioOut, file);
	PrintName ();
	printf ("Audio output file: %s\n", _audioOut);
}


bool mObjectDV::DecodeInit (void)
{
	bool results [5];
	bool result = true;
	results [0] = RegisterCodecs ();
	results [1] = SearchStreams ();
	results [2] = SearchCodecs ();
	results [3] = OpenCodecs ();
	results [4] = AllocBuffers ();
	//results [5] = CollectInfos ();

#ifdef DEBUG
	PrintName ();
	printf ("Init status: [");
	int i = 0;

	for (int i = 0; i < 5; i++)
		printf ("%d", result [i]);
	printf ("]\n");
#endif

	for (int i = 0; i < 5; i++)
		if (results [i] == false)
			return false;

	return result;
}


bool mObjectDV::RegisterCodecs (void)
{
    PrintName ();
	printf ("Registering codecs and formats\n");
	av_register_all ();


	if (av_open_input_file (&_formatCtx, _file, NULL, 0, NULL) != 0)
	{
		PrintName ();
		printf ("Error: could not open file \"%s\"\n", _file);
		return false;
	}
	PrintName ();
	printf ("File \"%s\" successfully opened\n", _file);
	
	return true;
}


bool mObjectDV::SearchStreams (void)
{
	if (av_find_stream_info (_formatCtx) < 0)
	{
		PrintName ();
		printf ("Error: could not find stream information\n");
		return false;
	}
	PrintName ();
	printf ("Stream information found:\n");
	dump_format (_formatCtx, 0, _file, false);
	
	PrintName ();
	printf ("Searching for Streams:\n");

	if (_enableVideo)
	{
		_streamVideoID =- 1;
		for (int i = 0; i < _formatCtx->nb_streams; i++)
			if (_formatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
			{
				_streamVideoID = i;
				break;
			}
		
		if (_streamVideoID == -1)
		{
			printf ("  Video Stream: not found\n");
			_enableVideo = false;
		}
		else
			printf ("  Video Stream: #0.%d\n", _streamVideoID);
	}

	if (_enableAudio)
	{
		_streamAudioID =- 1;
		for (int i = 0; i < _formatCtx->nb_streams; i++)
			if (_formatCtx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
			{
				_streamAudioID = i;
				break;
			}

		if (_streamAudioID == -1)
		{
			printf ("  Audio Stream: not found\n");
			_enableAudio = false;
		}
		else
			printf ("  Audio Stream: #0.%d\n", _streamAudioID);
	}

	return true;
}


bool mObjectDV::SearchCodecs (void)
{
	PrintName ();
	printf ("Searching for Codecs:\n");

	if (_enableVideo)
	{
		assert (_streamVideoID > -1);
		_codecCtxVideo = _formatCtx->streams [_streamVideoID]->codec;
	}
	if (_enableAudio)
	{
		assert (_streamAudioID > -1);
	    _codecCtxAudio = _formatCtx->streams [_streamAudioID]->codec;
	}


    if (_enableVideo)
	{
		_codecVideo = avcodec_find_decoder (_codecCtxVideo->codec_id);
		if (_codecVideo == NULL)
		{
			printf ("  Error: Video Codec not found\n");
			return false; 
		}
		printf ("  Video Codec: %s\n", _codecVideo->name); 	
	}

	if (_enableAudio)
	{
		_codecAudio = avcodec_find_decoder (_codecCtxAudio->codec_id);
		if (_codecAudio == NULL)
		{
			printf ("  Error: Audio Codec not found\n");
			return false;
		}
		printf ("  Audio Codec: %s\n", _codecAudio->name); 
				
		if (streamAudio == NULL)
			streamAudio = new mObjectPCM<int16_t> (_codecCtxAudio->channels, MPCM_STREAM_BIG);
			//streamAudio = new mObjectPCM<signed short> (_codecCtxAudio->channels, MPCM_STREAM_BIG);
	}

	return true;
}


bool mObjectDV::OpenCodecs (void)
{
	PrintName ();
	printf ("Opening Codecs:\n");

	if (_enableVideo)
	{
		if (avcodec_open (_codecCtxVideo, _codecVideo) < 0)
		{
			printf ("  Error: could not open Video Codec\n");
			return false;
		}
		printf ("  Video Codec is ready\n");
   	}
		
	if (_enableAudio)
	{
		if (avcodec_open (_codecCtxAudio, _codecAudio) < 0)
		{
			printf ("  Error: could not open Audio Codec\n");
			return false;
		}
		printf ("  Audio Codec is ready\n");
	}
	return true;
}


bool mObjectDV::AllocBuffers (void)
{

	PrintName ();
	printf ("Allocating buffers:\n");

	if (_enableVideo)
	{

		_frameYUV = avcodec_alloc_frame ();
		_frameRGB = avcodec_alloc_frame ();

		if (_frameRGB == NULL)
			printf ("  Error: Video Frame RGB not allocated\n");
		
		if (_frameYUV == NULL)
			printf ("  Error: Video Frame YUV not allocated\n");

		_frameYUVBytes = avpicture_get_size (PIX_FMT_RGB24, 
		                                     _codecCtxVideo->width, 
											 _codecCtxVideo->height);

		_bufferVideo= new uint8_t [_frameYUVBytes];
		printf ("  Video buffer %4d KB\n", _frameYUVBytes/1024);
		
		avpicture_fill ((AVPicture *)_frameRGB, 
				        _bufferVideo, 
						PIX_FMT_RGB24, 
						_codecCtxVideo->width, 
						_codecCtxVideo->height);
	}

	if (_enableAudio)
	{
		_bufferAudio = (int16_t *)malloc (AVCODEC_MAX_AUDIO_FRAME_SIZE); 

		printf ("  Audio buffer %4d KB\n", sizeof (int16_t) * AVCODEC_MAX_AUDIO_FRAME_SIZE / 1024);
	}

	return true;
}
		

bool mObjectDV::CollectInfos (void)
{
	PrintName ();
	printf ("Collecting parameters\n");
	
	_framesVideo = 0;
	_framesAudio = 0;

	/*
	 * Please read the comment at the beginning of 
	 * mObjectDV::DecodeStart
	 */
	av_seek_frame (_formatCtx, -1, AV_TIME_BASE, 1);


    while (av_read_frame (_formatCtx, &_packet) >= 0)
    {
        if (_packet.stream_index == _streamVideoID && _enableVideo)
			_framesVideo++;
		else if (_packet.stream_index == _streamAudioID && _enableAudio)
			_framesAudio++;
		 
	}
	

	if (_enableVideo)
		printf ("  %d Video Frames\n", _framesVideo);
	if (_enableAudio)
		printf ("  %d Audio Frames\n", _framesAudio);

	return (_framesAudio > 0 && _framesVideo > 0);
}


void mObjectDV::SetupAudio (bool Decode, bool Export)
{
	_decodeAudio = Decode;
	_exportAudio = Export;
	
	if (_enableAudio)
	{
		PrintName ();
		printf ("Audio Settings:\n");
		printf ("  Decode %s, Export %s\n", 
				_decodeAudio ? "Enabled" : "Disabled", 
				_exportAudio ? "Enabled" : "Disabled");
	}
}


void mObjectDV::SetupVideo (bool Decode, bool Export)
{
	_decodeVideo = Decode;
	_exportVideo = Export;
	
	if (_enableVideo)
	{
		PrintName ();
		printf ("Video Settings:\n");
		printf ("  Decode %s, Export %s\n", 
				_decodeVideo ? "Enabled" : "Disabled", 
				_exportVideo ? "Enabled" : "Disabled");
	}
}

int64_t mObjectDV::GetPTS (int streamID)
{
	int64_t PTS;
	PTS = av_rescale_q (_packet.pts, _formatCtx->streams[streamID]->time_base, AV_TIME_BASE_Q);
	return PTS;
}

bool mObjectDV::DecodeAudio (void)
{
	PrintName ();
	printf ("Decoding Audio\n");
	
	if (_enableAudio == false)
	{
		printf ("  Error: Audio disabled, decoding not possible\n");
		return false;
	}
	if (_audioOut == NULL && _exportAudio)
	{
		printf ("  Error: Audio output not set\n");
		return false;
	}

	int sampleSize 	   = sizeof (int16_t);
	int bytesRead      = 0;
	int bytesExpected  = 0;
	int framesDecoded  = 0;
	int SamplesDecoded = 0;
	
	int64_t PTS;
	int 	streamID;

	streamID = Seek ();
	
	while (av_read_frame (_formatCtx, &_packet) >= 0)
	{
		if (_packet.stream_index == _streamAudioID)
		{
			PTS = GetPTS (_streamAudioID);
			ShowProgress (PTS, framesDecoded);	
			//if (_seekT0 > -1 || _seekT1 > -1)
			if (_seekT1 > -1)
				if (PTS >= _seekT1)
					break;

			bytesRead = avcodec_decode_audio (_codecCtxAudio, 
					_bufferAudio,
					&bytesExpected, 
					_packet.data, 
					_packet.size);

			if (bytesExpected == bytesRead)
			{
				framesDecoded++;
				SamplesDecoded += _packet.size/sampleSize;
			}
			else
			{
				printf ("Error decoding\n");
				return false;
			}

			switch (_codecCtxAudio->channels)
			{
				case 1:
					streamAudio->AddSamplesM (_bufferAudio, (int)(bytesRead/sampleSize));
					break;
				case 2:
					streamAudio->AddSamplesLR (_bufferAudio, (int)(bytesRead/sampleSize));
					break;
			}
		}

	}
	cout << endl << flush;
	printf ("  %d Frames, %d Samples\n", framesDecoded, SamplesDecoded);
	
	/*
	 * Trimming audio streams
	 */
	streamAudio->Trim ();
	/*
	 * Exporting RAW audio
	 */
	if (_exportAudio)
	{
		PrintName ();
		printf ("Exporting Audio:\n");
		printf ("  File   %s\n", _audioOut);
		
		if (strstr (_audioOut, ".pcm") != NULL || strstr (_audioOut, ".PCM") != NULL)
		{
			printf ("  Codec  PCM via mObjectPCM\n");

			streamAudio->ExportPCM (_audioOut);
		}
		else
		{
			printf ("  Codec  FFMPEG via mObjectEncoder\n");
			
			mObjectEncoder *encoder = new mObjectEncoder;
			unsigned int samples = 0;
			int16_t *bufferL = NULL;
			int16_t *bufferR = NULL;
			streamAudio->ExportBuffers (&bufferL, &bufferR, samples);
			encoder->SetupAudioIn (bufferL, bufferR, samples, 48000, 2);
			encoder->SetupAudioOut ();
			encoder->EncodeAudio (_audioOut);
			delete encoder;
		}
	}
	return true;
}


bool mObjectDV::DecodeVideo (void)
{
	PrintName ();
	printf ("Decoding Video\n");

	if (_enableVideo == false)
	{
		printf ("  Error: Video disabled, decoding not possible\n");
		return false;
	}
	if (_videoOut == NULL) //&& _exportVideo (pls fix)
	{
		printf ("  Error: Video output not set\n");
		return false;
	}
	
	_imageProcAlloc = false;

	int bytesRead      = 0;
	int bytesExpected  = 0;
	int framesDecoded  = 0;
	
	int64_t PTS;
	int		streamID;
	
	streamID = Seek ();

	while (av_read_frame (_formatCtx, &_packet) >= 0)
	{
		if (_packet.stream_index == _streamVideoID)
		{
			PTS = GetPTS (_streamVideoID);
			ShowProgress (PTS, framesDecoded);
			//if (_seekT0 > -1 || _seekT1 > -1)
			if (_seekT1 > -1)
				if (PTS >= _seekT1)
					break;
			
			bytesRead = avcodec_decode_video (_codecCtxVideo, 
					_frameYUV, 
					&bytesExpected, 
					_packet.data, 
					_packet.size);


			if (bytesExpected)
			{
				framesDecoded++;
				img_convert ((AVPicture *)_frameRGB, 
						PIX_FMT_RGB24, 
						(AVPicture *)_frameYUV, 
						_codecCtxVideo->pix_fmt, 
						_codecCtxVideo->width, 
						_codecCtxVideo->height);
			}
			else
			{
				printf ("Error decoding\n");
				return false;
			}
			if (_decodeVideo)
			{
				if (_imageProcAlloc	== false)
				{
					_imageProc->New (_codecCtxVideo->width, _codecCtxVideo->height, MIMAGE_RGB);
					_imageProcAlloc	= true;
				}
				assert (_imageProc->DumpBuffer (_frameRGB->data [0]) == true);
				
				if (_exportVideo)
				{ 
					if (strcmp (_videoOut, "") == 0)
						_imageProc->WritePPM ("frame", framesDecoded);
					else
					{
						memset (_imageProcName, 0, MOBJECTDV_FNSIZE);
						strcat (_imageProcName, _videoOut);
						strcat (_imageProcName, "/frame");
						_imageProc->WritePPM (_imageProcName, framesDecoded);
					}
				}
			}

		}
	}
	cout << endl << flush;
	//printf ("  %d Frames\n", framesDecoded);

	if (_imageProc != NULL && _imageProcAlloc == true)
	{
		delete _imageProc;
		_imageProc = new mObjectIMG;
	}

	return true;
}




void mObjectDV::PrintDetails (void)
{
	if (_enableAudio)
	{
		PrintName ();
		printf ("Audio stream details:\n");
		printf ("  %d Frames (Decoded), %d/%d TB, %d kbps, %d kHz, %d Chs.\n",
				_codecCtxAudio->frame_number,
				_codecCtxAudio->time_base.num,
				_codecCtxAudio->time_base.den,
				(int)(_codecCtxAudio->bit_rate/1000),
				(int)(_codecCtxAudio->sample_rate/1000),
				_codecCtxAudio->channels);
	}

	if (_enableVideo)
	{
		PrintName ();
		printf ("Video stream details:\n");
		printf ("  %d Frames (Decoded), %d/%d TB, %d kbps, %dx%d px\n",
				_codecCtxVideo->frame_number,
				_codecCtxVideo->time_base.num,
				_codecCtxVideo->time_base.den,
				(int)(_codecCtxVideo->bit_rate/1000),
				_codecCtxVideo->width, 
				_codecCtxVideo->height);
	}
}

void mObjectDV::Test (void)
{
	PrintName ();
	printf ("Entering test\n");

	PrintName ();
	printf ("Leaving test\n");
}

inline void mObjectDV::PTS2Time (int64_t t, int &h, int &m, int &s)
{
	cout << t << endl;
	s  =  t / AV_TIME_BASE;
	//int us =  t % AV_TIME_BASE;
	m  =  s / 60;
	s  %= 60;
	h  =  m / 60;
	m  %= 60;
	//int ds = (10 * us) / AV_TIME_BASE;
}

inline void mObjectDV::Time2PTS (int64_t &t, int h, int m, int s)
{
	t = (60 * (60 * h + m) + s) * AV_TIME_BASE;
}
		
bool mObjectDV::DecodeFrom (int h, int m, int s)
{
	PrintName ();
	printf ("Decoding T0: %.2d:%.2d:%.2d", h, m, s);
	
	int64_t seekFrom = 0;
	Time2PTS (seekFrom, h, m, s);
	if (seekFrom >= 0 && seekFrom < _formatCtx->duration)
	{
		_seekT0 = seekFrom;
		printf (" (PTS %d)\n", (int)_seekT0);
	}
	return true;
}
		

bool mObjectDV::DecodeTo (int h, int m, int s)
{
	if (_seekT0 == -1)
		DecodeFrom (0, 0, 0);

	PrintName ();
	printf ("Decoding T1: %.2d:%.2d:%.2d", h, m, s);
	
	int64_t seekTo = 0;
	Time2PTS (seekTo, h, m, s);
	if (seekTo > _seekT0 && seekTo < _formatCtx->duration)
	{
		_seekT1 = seekTo;
		printf (" (PTS %d)\n", (int)_seekT1);
	}
	return true;
}


int mObjectDV::Seek (void)
{
	int StreamID = -1;

	if (_streamVideoID > -1)
		StreamID = _streamVideoID;
	else if (_streamAudioID > -1)
		StreamID = _streamAudioID;

	if (_seekT1 == -1 && _seekT0 == -1)
		av_seek_frame (_formatCtx, 
				StreamID,
				av_rescale_q (0, AV_TIME_BASE_Q, _formatCtx->streams[StreamID]->time_base),
				AVSEEK_FLAG_BACKWARD);
	else if (_seekT0 > 0)
		av_seek_frame(_formatCtx, 
				StreamID,
				av_rescale_q (_seekT0, AV_TIME_BASE_Q, _formatCtx->streams[StreamID]->time_base),
				AVSEEK_FLAG_BACKWARD);
	/*
	else
	{
		printf ("Error: seek unsuccessfull\n");
		return -1;
	}
	*/
	return StreamID;
	//else if ((_seekT1 > _seekT0) || (_seekT0 >= 0 && _seekT1 == -1))
	//else if (_seekT1 > _seekT0 || _seekT0 >= 0)
}


/* This function was found in
 * ffpocket v0.63
 */
void mObjectDV::ShowProgress (int64_t timepos, int counter) 
{
	int hours, mins, secs, us;
	secs = timepos / AV_TIME_BASE;
	us = timepos % AV_TIME_BASE;
	mins = secs / 60;
	secs %= 60;
	hours = mins / 60;
	mins %= 60;
	fprintf(stderr, "  Decoding at time: %02d:%02d:%02d.%01d  Frame: %06d  \r",
		hours, mins, secs, (10 * us) / AV_TIME_BASE, counter);
	//sleep (1);
	fflush(stderr);

}
#endif
		
// ---------------------------- DISCONTINUED ---------------------------- 

//void SaveStreamPCM (uint16_t *buffer, int blength, char *filename = "Data/audio.pcm");
//void SaveFramePPM (AVFrame *FrameYUV, int width, int height, int iFrame, char *pFrame = "frame_");

/*	
void mObjectDV::SaveFramePPM (AVFrame *Frame, int width, int height, int iFrame, char *pFrame)
{
	assert (Frame != NULL);
	assert (width > 0);
	assert (height > 0);
	assert (iFrame >= 0);
	
	cout << "saving " << iFrame << endl;
    FILE *file;
    char filename [32];

    // Open file
    sprintf (filename, "%s%d.ppm", pFrame, iFrame);
    file = fopen (filename, "wb");
	assert (file != NULL);

    // Write header
    fprintf (file, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for (int y = 0; y < height; y++)
        fwrite (Frame->data[0] + y*Frame->linesize[0], 1, width*3, file);

    // Close file
    fclose (file);
}
*/

/*
void mObjectDV::SaveStreamPCM (uint16_t *buffer, int blength, char *filename)
{
	assert (blength > 0);
	assert (buffer != NULL);
	assert (filename != NULL);


    FILE *file;
	file = fopen (filename, "w");
	assert (file != NULL);
	
	for (int s = 0; s < (int)(blength/sizeof (uint16_t)); s = s + 2)
		fprintf (file, "%d %d\n", (signed short)buffer [s], (signed short)buffer [s + 1]);
	fclose (file);
}

bool mObjectDV::DecodeStart (int FrameFrom, int FrameTo)
{
	PrintName ();
	printf ("Entering Decode Loop\n");
	
	av_seek_frame (_formatCtx, -1, AV_TIME_BASE, 1);
	
	int frame = 0;
	int VideoFrames = 0;
	int AudioFrames = 0;

	int Tbytes = 0;

	int SamplesExpected = 0;

	bool DecodeInterval = false;

	if (FrameFrom != -1 && FrameTo != -1)
		if (_framesVideo == _framesAudio)
			if (FrameFrom > 0 && FrameFrom < _framesVideo)
				if (FrameTo > 0 && FrameTo > FrameFrom)
					DecodeInterval = true;
				else
					return false;
			else
				return false;
		else
			return false;

#ifdef USE_MAGICK
	Image *ImageMGK = NULL;
	char *ImageMGK_name = new char [128];
#endif

	while (av_read_frame (_formatCtx, &_packet) >= 0)
    {
        if (_packet.stream_index == _streamVideoID && _enableVideo)
        {
			VideoFrames++;
			if (!DecodeInterval || (DecodeInterval && VideoFrames >= FrameFrom && VideoFrames <= FrameTo))
			{
				avcodec_decode_video (_codecCtxVideo, 
									  _frameYUV, 
									  &_frameYUVFinished, 
									  _packet.data, 
									  _packet.size);

				if (_frameYUVFinished)
				{
					img_convert ((AVPicture *)_frameRGB, 
								 PIX_FMT_RGB24, 
								 (AVPicture *)_frameYUV, 
								 _codecCtxVideo->pix_fmt, 
								 _codecCtxVideo->width, 
								 _codecCtxVideo->height);

					if (_decodeVideo)
					{
#ifndef USE_MAGICK
						(streamVideo->_data [VideoFrames]).New (_codecCtxVideo->width,
								_codecCtxVideo->height, 
								MIMAGE_RGB);
						
						(streamVideo->_data [VideoFrames]).DumpBuffer (_frameRGB->data[0]); 

						if (_exportVideo)
							(streamVideo->_data [VideoFrames]).SavePPM ("Data/frame", VideoFrames);
#else

						if (ImageMGK == NULL)
							ImageMGK = new Image (_codecCtxVideo->width, 
												  _codecCtxVideo->height, 
												  "RGB",
												  CharPixel,
												  _frameRGB->data[0]);
						else
							ImageMGK->read (_codecCtxVideo->width,
											_codecCtxVideo->height,
											"RGB",
											CharPixel,
											_frameRGB->data[0]);
						
						streamVideo.push_front (*ImageMGK);
						
						if (_exportVideo)
						{ 
							memset (ImageMGK_name, 0, 128);
							sprintf (ImageMGK_name, "Data/MGK_frame_%.6d.bmp", VideoFrames);

							ImageMGK->write (ImageMGK_name);
						}
#endif
					}
				}
#ifdef DEBUG_STREAM
				PrintName ();
				printf ("Video Packet [%.8d]: %8d %8d\n", VideoFrames, 
														 (int)_packet.pts, 
														 (int)_packet.duration);
#endif
			}
		}
		else if (_packet.stream_index == _streamAudioID && _enableAudio)
		{
			AudioFrames++;

			if (!DecodeInterval || (DecodeInterval && AudioFrames >= FrameFrom && AudioFrames <= FrameTo))
			{
				_framePCMLenght = avcodec_decode_audio (_codecCtxAudio, 
														_bufferAudio,
														&_framePCMBytes, 
														_packet.data, 
														_packet.size);

				cout << "_packet.size    " << _packet.size << " bytes" << endl;
				cout << "_packet.size    " << _packet.size/2 << " samples" << endl;
				cout << "_framePCMLenght " << _framePCMLenght << " bytes" << endl;
			
				break;
				
				assert (_framePCMBytes == _framePCMLenght);

				if (_framePCMLenght < 0 || _framePCMBytes <= 0)
				{
					cout << "  Error: while decoding\n";
					return false;
				}
				
#ifdef DEBUG_STREAM
				PrintName ();
				printf ("Audio Packet [%.8d]: %8d %8d\n", AudioFrames, 
														 (int)_packet.pts, 
														 (int)_packet.duration);
#endif
				if (_decodeAudio)
				{
					switch (_codecCtxAudio->channels)
					{
						case 1:
							streamAudio->AddSamplesM ((uint8_t*)_bufferAudio, (int)(_framePCMBytes/sizeof(int16_t)));
							break;
						case 2:
							streamAudio->AddSamplesLR ((uint8_t*)_bufferAudio, (int)(_framePCMBytes/sizeof(int16_t)));
							break;
					}
				}
				// Paranoid
				assert( _packet.size - _framePCMBytes == 0);
				SamplesExpected += _packet.size/sizeof (int16_t);
			}
		}

#ifdef PROGRESS
		if (frame == 0)
			cout << " Working... [=" << flush;
		else if (frame % 250 == 0)
			cout << "=" << flush;
#endif
	
		frame++;
        av_free_packet (&_packet);
		
		if (AudioFrames == FrameTo && VideoFrames == FrameTo)
			break;
    }

#ifdef PROGRESS
	cout << "] Done!" << endl;
#endif
	if (_enableAudio)
	{
		PrintName ();
		printf ("Expected samples per channel: %d\n", SamplesExpected / _codecCtxAudio->channels);

		cout << sizeof (int16_t) << endl;
		cout << sizeof (uint8_t) << endl;

		if (_exportAudio)
		{
			//streamAudio->PrintDetails ();
			streamAudio->TrimStream ();
			//streamAudio->PrintDetails ();
			streamAudio->ExportASCII ();
		}
	}

	PrintName ();
	printf ("Leaving Decode Loop\n");

#ifdef DEBUG
	PrintName ();
	printf ("Detected:\n");
	if (_enableVideo)
		printf ("  %d Video Frames\n", VideoFrames);
	if (_enableAudio)
		printf ("  %d Audio Frames\n", AudioFrames);
#endif	
	
	cout << ">\tAudio Bytes Read:        " << Tbytes << endl;
	cout << ">\tAudio Samples Read (KO): " << Tbytes/2 << endl;
	cout << ">\tAudio Samples Read (OK): " << Tbytes/2 + _codecCtxAudio->sample_rate << endl;
	return true;
}
*/

/*
 * DecodeVideo: MGK
#ifdef USE_MAGICK
				if (ImageMGK == NULL)
					ImageMGK = new Image (_codecCtxVideo->width, 
							_codecCtxVideo->height, 
							"RGB",
							CharPixel,
							_frameRGB->data[0]);
				else
					ImageMGK->read (_codecCtxVideo->width,
							_codecCtxVideo->height,
							"RGB",
							CharPixel,
							_frameRGB->data[0]);

				streamVideo->push_front (*ImageMGK);

				if (_exportVideo)
				{ 
					memset (ImageMGK_name, 0, 128);
					sprintf (ImageMGK_name, "Data/MGK_frame_%.6d.bmp", framesDecoded);

					ImageMGK->write (ImageMGK_name);
				}
#else
*/
