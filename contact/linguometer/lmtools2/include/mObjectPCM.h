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

/* This class has few pros and few cons.
 * o Pros: 
 *   + simple to use
 *   + data is handled correctly, with no bugs ;-)
 *   + good abstraction, mainly used in 
 *     codecs/decoders
 * o Cons:
 *   - slow access to the data
 *   - for loops should be replaced my memcpy etc.
 *   - the interface is not compliant with the other
 *     lmtools2 interfaces
 *   - dynamic allocation of buffers is not present,
 *     that means: this class uses a lot of ram.
 *     Use Trim() to trim your data and release some memory.
 */
#ifndef MOBJECTPCM_H
#define MOBJECTPCM_H

#include "mInclude.h"
#include "mClass.h"

#define MPCM_STREAM_SIZE	AVCODEC_MAX_AUDIO_FRAME_SIZE

/* I believe that a 512 MB (per channel) 
 * buffer for audio processing is more 
 * than what we'll ever need... 
 */
#define MPCM_STREAM_TINY	64		
#define MPCM_STREAM_SMALLER	128		
#define MPCM_STREAM_SMALL	256		// Memory:  65536 KB
#define MPCM_STREAM_STD		512		// Memory: 131072 KB
#define MPCM_STREAM_BIG		1024	// Memory: 262144 KB
#define MPCM_STREAM_HUGE	2048	// Memory: 524288 KB

#define MPCM_STEREO		2
#define MPCM_MONO		1


/* MPCM_* should be used
 * carefully (look at constructor)
 */
#define MPCM_R			0
#define MPCM_L			1
#define MPCM_M			2

template <typename PCMBits>  
struct mPCM_t {
	PCMBits *L;
	PCMBits *R;
	unsigned int *samples;
};


template <typename PCMBits>  
class mObjectPCM : public mClass
{
	public:
		mObjectPCM (int channels = MPCM_STEREO, int samples = MPCM_STREAM_SMALL, bool _verbose = false);
		~mObjectPCM ();

		/* This method adds RAW coding PCM
		 * samples to private streams.
		 * 
		 * Example: 
		 * Sample:  0 1 2 3 4 5 6 7 8 9 ...
		 * Channel: L R L R L R L R L R ...
		 *
		 * Note: this method is slow since a for cycle 
		 *       is used for heavy-duty PCM RAW to PCM L+R
		 *       stream split.
		 *       I believe a for cycle is necessary anyway.
		 */
		bool AddSamplesLR (PCMBits *buffer, int samples);

		/* This method adds Right Channel PCM
		 * samples to private streams.
		 * 
		 * Example: 
		 * Sample:  0 1 2 3 4 5 6 7 8 9 ...
		 * Channel: R R R R R R R R R R ...
		 */
		bool AddSamplesR (PCMBits *buffer, int samples);
		
		/* This method adds Left Channel PCM
		 * samples to private streams.
		 * 
		 * Example: 
		 * Sample:  0 1 2 3 4 5 6 7 8 9 ...
		 * Channel: L L L L L L L L L L ...
		 */
		bool AddSamplesL (PCMBits *buffer, int samples);
		
		/* This method adds Mono PCM
		 * samples to private streams.
		 * 
		 * Example: 
		 * Sample:  0 1 2 3 4 5 6 7 8 9 ...
		 * Channel: M M M M M M M M M M ...
		 */
		bool AddSamplesM (PCMBits *buffer, int samples);

		inline PCMBits getR (unsigned int s) {
			assert (_channels == MPCM_STEREO);
			assert (s >= 0 || s < (unsigned int)_samples);
			return _stream [R][s];
		}
		
		inline PCMBits getL (unsigned int s) {
			assert (_channels == MPCM_STEREO);
			assert (s >= 0 || s < (unsigned int)_samples);
			return _stream [L][s];
		}
		
		unsigned int getSamples (void) {
			return _samples;
		}

		void PrintDetails (void);
		void Trim (void);
		void ExportPCM (char *filename = "audio.pcm");
		bool ExportBuffers (PCMBits **bufferL, PCMBits **bufferR, unsigned int &samples);	
		bool ExportBuffer (PCMBits **bufferM, unsigned int &samples);	

	private:
		PCMBits ** AllocStream (int channels, int samples);
		void DeallocStream (PCMBits **stream, int channels);
		bool AddSamplesCH (PCMBits *buffer, int samples, int channel);

	private:
		PCMBits		**_stream;
		int			*_streamIDX;


		int 		_samples;
		int			_channels;

		int			R;
		int			L;
		int			M;

		bool		_ready;
		bool		_verbose;
};


template <typename PCMBits>
mObjectPCM<PCMBits>::mObjectPCM (int channels, int samples, bool verbose)
{
	SetName ("mObjectPCM");

	_verbose = verbose;

	if (_verbose) {
		PrintName ();
		printf ("Starting %s\n", GetName ());
	}
	assert (channels == MPCM_STEREO || channels == MPCM_MONO);
	assert (samples > 0);

	_channels = channels;
	_samples  = samples * MPCM_STREAM_SIZE ;
	
	if (_verbose) {
		PrintName ();
		printf ("Allocating PCM streams\n");
		printf ("  %d Bits, %d Ch., %d Samples, %d KB\n",  
				8*sizeof (PCMBits), 
				_channels, 
				_samples, 
				(int)(sizeof (PCMBits)*_samples*_channels/1024));
	}	
	// Allocating stream index for I/O
	_streamIDX = new int [_channels];

	// Allocating channels
	_stream = AllocStream (_channels, _samples);

	// Cleaning channels
	for (int ch = 0; ch < _channels; ch++)
		memset (_stream [ch], (PCMBits)0, _samples);

	// Setting up stream and idx pointers
	if (channels == MPCM_STEREO)
	{
		L =  0;
		R =  1;
		M = -1;

		_streamIDX [L] = 0;
		_streamIDX [R] = 0;
	}
	else if (channels == MPCM_MONO)
	{
		L = -1;
		R = -1;
		M =  0; 
		
		_streamIDX [M] = 0;
	}
}


template <typename PCMBits>  
mObjectPCM<PCMBits>::~mObjectPCM ()
{
	if (_verbose) {
	PrintName ();
	printf ("Destroying %s\n", GetName ());
	}

	DeallocStream (_stream, _channels);
}


template <typename PCMBits>  
PCMBits ** mObjectPCM<PCMBits>::AllocStream (int channels, int samples)
{
	PCMBits **stream = NULL;
	
	stream = (PCMBits **)malloc (channels * sizeof (PCMBits **));
	assert (stream != NULL);
	for (int ch = 0; ch < channels; ch++)
	{
		stream [ch] = (PCMBits *)malloc (samples * sizeof (PCMBits *));
		assert (stream [ch] != NULL);
	}

	return stream;
}


template <typename PCMBits>  
void mObjectPCM<PCMBits>::DeallocStream (PCMBits **stream, int channels)
{
	for (int ch = 0; ch < channels; ch++)
	{
		if (stream [ch] != NULL)
			delete stream [ch];
	}
	delete stream;
}

	
template <typename PCMBits>  
bool mObjectPCM<PCMBits>::AddSamplesLR (PCMBits *buffer, int samples)
{
	if (_channels != MPCM_STEREO)
	{
		if (_verbose) {
			PrintName ();
			printf ("Error: this (dumb) method requires a Stereo Stream\n");
		}
		return false;
	}
	
	if (buffer == NULL)
	{
		if (_verbose) {
			PrintName ();
			printf ("Error: buffer is invalid\n");
		}
		return false;
	}
	
	if (samples <= 0)
	{
		if (_verbose) {
			PrintName ();
			printf ("Error: samples should be > 0\n");
		}
		return false;
	}
	
	for (int s = 0; s < samples; s += 2)
	{
		_stream [L][_streamIDX[L]++] = buffer [s + 0];
		_stream [R][_streamIDX[R]++] = buffer [s + 1];
	}

#ifdef DEBUG
	for (int s = 0; s < _streamIDX[L] + 2; s++)
	{
		cout << s << ": ";
		cout << _stream [L][s];
		cout << " ";
		cout << _stream [R][s] << endl;
	}	
#endif
	return true;
}


template <typename PCMBits>  
bool mObjectPCM<PCMBits>::AddSamplesR (PCMBits *buffer, int samples)
{
	if (_channels != MPCM_STEREO)
	{
		if (_verbose) {
			PrintName ();
			printf ("Error: this (dumb) method requires a Stereo Stream\n");
		}
		return false;
	}

	AddSamplesCH (buffer, samples, R);

	return true;
}


template <typename PCMBits>  
bool mObjectPCM<PCMBits>::AddSamplesL (PCMBits *buffer, int samples)
{
	if (_channels != MPCM_STEREO)
	{
		if (_verbose) {
			PrintName ();
			printf ("Error: this (dumb) method requires a Stereo Stream\n");
		}
		return false;
	}
	
	AddSamplesCH (buffer, samples, R);
	
	return true;
}


template <typename PCMBits>  
bool mObjectPCM<PCMBits>::AddSamplesM (PCMBits *buffer, int samples)
{
	if (_channels != MPCM_MONO)
	{
		if (_verbose) {
			PrintName ();
			printf ("Error: this (dumb) method requires a Mono Stream\n");
		}
		return false;
	}
	
	AddSamplesCH (buffer, samples, M);
	
	return true;
}

template <typename PCMBits>  
bool mObjectPCM<PCMBits>::AddSamplesCH (PCMBits *buffer, int samples, int channel)
{
	if (buffer == NULL)
	{
		if (_verbose) {
			PrintName ();
			printf ("Error: buffer = NULL\n");
		}
		return false;
	}
	
	if (samples <= 0)
	{
		if (_verbose) {
			PrintName ();
			printf ("Error: samples should be > 0\n");
		}
		return false;
	}
	

	for (int s = 0; s < samples; s++)
		_stream [channel][_streamIDX[channel]++] = buffer [s];

	return true;
}


template <typename PCMBits>  
void mObjectPCM<PCMBits>::PrintDetails (void)
{

	if (_channels == MPCM_STEREO)
	{
		PrintName ();
		printf ("PCM Informations:\n");
		printf ("  Channels:    %d\n", _channels);
		printf ("  Samples L:   %d/%d\n", _streamIDX[L], _samples);
		printf ("  Samples R:   %d/%d\n", _streamIDX[R], _samples);
		printf ("  Size L:      %d KB/%d KB\n", _streamIDX[L]*sizeof (PCMBits)/1024, _samples*sizeof (PCMBits)/1024);
		printf ("  Size R:      %d KB/%d KB\n", _streamIDX[R]*sizeof (PCMBits)/1024, _samples*sizeof (PCMBits)/1024);


	}
	else if  (_channels == MPCM_MONO)
	{
		PrintName ();
		printf ("PCM Informations:\n");
		printf ("  Channels:    %d\n", _channels);
		printf ("  Samples M:   %d/%d\n", _streamIDX[M], _samples);
		printf ("  Size M:      %d KB/%d KB\n", _streamIDX[M]*sizeof (PCMBits)/1024, _samples*sizeof (PCMBits)/1024);
	}
}

template <typename PCMBits>  
void mObjectPCM<PCMBits>::Trim (void)
{
	if (_channels == MPCM_STEREO)
	{
		assert (_streamIDX[R] == _streamIDX[L]);
	
		if (_verbose) {
			PrintName ();
			printf ("Trimming Stream:\n");
			printf ("  %d Total, %d Used, %d Wasted\n", 
					_channels * _samples, 
					_channels * _streamIDX[L],
					_channels * (_samples - _streamIDX[L]));
		}	
		PCMBits **StreamTemp = AllocStream (_channels, _streamIDX[L]);
	
		for (int s = 0; s < _streamIDX[R]; s++)
		{
			StreamTemp[R][s] = _stream[R][s];
			StreamTemp[L][s] = _stream[L][s];
		}

		DeallocStream (_stream, _channels);
		_stream = StreamTemp;
		_samples = _streamIDX[R];
	}
	else if  (_channels == MPCM_MONO)
	{
		if (_verbose) {
			PrintName ();
			printf ("Trimming Stream:\n");
			printf ("  %d Total, %d Used, %d Wasted\n", 
					_samples, 
					_streamIDX[M],
					_samples - _streamIDX[M]);

			printf ("  Used Samples:  %d\n", _streamIDX[M]);
			printf ("  Total Samples: %d\n", _samples);
			printf ("  Difference:    %d\n", _samples - _streamIDX[L]);
		}	
		PCMBits **StreamTemp = AllocStream (_channels, _streamIDX[M]);
	
		for (int s = 0; s < _streamIDX[M]; s++)
			StreamTemp[M][s] = _stream[M][s];

		DeallocStream (_stream, _channels);
		_stream = StreamTemp;
		_samples = _streamIDX[M];
	}
}


template <typename PCMBits>  
void mObjectPCM<PCMBits>::ExportPCM (char *filename)
{
	assert (filename != NULL);
	assert (_streamIDX[R] == _streamIDX[L]);

	FILE *fd;
	fd = fopen (filename, "w");
	assert (fd != NULL);
	
	if (_channels == MPCM_STEREO)
		for (int s = 0; s < _streamIDX[L]; s = s + 1)
			fprintf (fd, "%d %d\n", (signed short)_stream [L][s], (signed short)_stream [R][s]);
	else if  (_channels == MPCM_MONO)
		for (int s = 0; s < _streamIDX[M]; s = s + 1)
			fprintf (fd, "%d\n", (signed short)_stream [M][s]);

	fclose (fd);
}
		
template <typename PCMBits>  
bool mObjectPCM<PCMBits>::ExportBuffers (PCMBits **bufferL, PCMBits **bufferR, unsigned int &samples)
{
	/* Check:
	if (M != 0)
		return false;
	*/

	assert (*bufferR == NULL);
	assert (*bufferL == NULL);
	assert (_streamIDX[R] == _streamIDX[L]);

	samples = _streamIDX[L];

	(*bufferR) = (PCMBits *)malloc (samples * sizeof (PCMBits));
	(*bufferL) = (PCMBits *)malloc (samples * sizeof (PCMBits));

	for (unsigned int s = 0; s < samples; s++)
	{
		(*bufferL)[s] = _stream [L][s];
		(*bufferR)[s] = _stream [R][s];
	}

	return true;
}

template <typename PCMBits>  
bool mObjectPCM<PCMBits>::ExportBuffer (PCMBits **bufferM, unsigned int &samples)
{
	if (M != 0)
		return false;

	assert (*bufferM == NULL);

	samples = _streamIDX[M];

	(*bufferM) = (PCMBits *)malloc (samples * sizeof (PCMBits));

	for (unsigned int s = 0; s < samples; s++) 
		(*bufferM)[s] = _stream [M][s];

	return true;
}

#endif
