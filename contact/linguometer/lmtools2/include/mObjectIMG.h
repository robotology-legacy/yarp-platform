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

#ifndef MOBJECTIMG_H
#define MOBJECTIMG_H

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <math.h>
using namespace std;

#include "mClass.h"

#define MOBJECTIMG_STD_WIDTH	320
#define MOBJECTIMG_STD_HEIGHT	240
#define MOBJECTIMG_STD_BPP		8

#define MIMAGE_RGB				3
#define MIMAGE_G				1

#define MIMAGE_BLACK			0
#define MIMAGE_WHITE			255

#define MIMAGE_ON				255
#define MIMAGE_OFF				0	

#define MFILTER_SIZE			9

/* Positive filters are 
 * directional (Vert. + Horiz.)
 */
#define MFILTER_SOBEL			+1

/* Negative filters are 
 * a-directional masks
 */
#define MFILTER_NOISE_9			-1
#define MFILTER_NOISE_10		-2
#define MFILTER_NOISE_16		-3
#define MFILTER_M1				-4
#define MFILTER_LAPLACE_4		-5
#define MFILTER_LAPLACE_8		-6


#define MCH_RGB_R				0
#define MCH_RGB_G				1
#define MCH_RGB_B				2

#define MCH_G					0

#define MHIST_SIZE				256

struct mImage
{
	unsigned int w;			// Width
	unsigned int h; 		// Height
	unsigned int bpp;		// Bits Per Pixel
	unsigned char *data;	// Pixels
	bool binary;
	size_t bytes;
};

struct mHistG
{
	unsigned char data [MHIST_SIZE];
	unsigned char min;
	unsigned char max;
	unsigned char mean;
};

class mObjectIMG : public mClass
{
	public:
		mObjectIMG ();
		~mObjectIMG ();
		/*
		 * This method allocates a new mImage.
		 */
		void New (unsigned int w = MOBJECTIMG_STD_WIDTH, 
				  unsigned int h = MOBJECTIMG_STD_HEIGHT, 
				  unsigned int bpp = MOBJECTIMG_STD_BPP);


		/*
		 * _SetPixel RGB order
		 */
		void SetPixelRGB (unsigned int x, 
						  unsigned int y,
						  unsigned char R,
						  unsigned char G,
						  unsigned char B);
		/*
		 * _SetPixel BGR order. 
		 */
		void SetPixelBGR (unsigned int x, 
						  unsigned int y,
						  unsigned char B,
						  unsigned char G,
						  unsigned char R);
		
		/*
		 * _SetPixel Grey (for 8bpp images)
		 */
		void SetPixelG (unsigned int x, 
						unsigned int y, 
						unsigned char V);

		inline unsigned char GetPixelG (unsigned int x,
									    unsigned int y);

		inline bool CheckPixel (unsigned int x, unsigned int y, unsigned int c);
	
		/* Simple resize method. Without interpolation.
		 */
		bool Resize (unsigned int w, unsigned int h);

		/* Converts an RGB image to Greyscale
		 */
		bool Grey (void);

		bool IsGrey (void) {return (_image->bpp == MIMAGE_G);  }
		bool IsRGB  (void) {return (_image->bpp == MIMAGE_RGB);}
		bool IsAllocated (void) {return (_image != NULL && _image->data != NULL);}
		unsigned int GetChMax (void) {return (IsGrey () ? MCH_G : MCH_RGB_B);}

		/* Crop using rectangular region definition
		 */
		bool Crop (unsigned int x0, 
				   unsigned int y0, 
				   unsigned int w, 
				   unsigned int h);

		/* Negative of an image
		 * (RGB or G)..
		 */
		bool Negative (void);

		/* Single value threshold
		 */
		bool Threshold (unsigned int th);
		
		/* Two values threshold
		 */
		bool Threshold (unsigned int thL, unsigned int thH, unsigned int dp = 1);

		/* Dilate or erode a "binary" image. 
		 * mObjectIMG does not support "real binary" images.
		 * A binary image is a 8bpp image with just two values:
		 * 0 (black) and 255 (white).
		 * You can specify what is "on" (information) and what is 
		 * "off" (background)
		 */
		bool Dilate (unsigned char on = MIMAGE_WHITE, unsigned char off = MIMAGE_BLACK);
		bool Erode (unsigned char on = MIMAGE_WHITE, unsigned char off = MIMAGE_BLACK);

		bool Opening (unsigned int times = 1);
		bool Closing (unsigned int times = 1);

		unsigned char Max (void);
		unsigned char Min (void);
		unsigned char Mean (void);
		/*
		 * DumpBuffer allows to dump an image buffer inside 
		 * the private mImage image object.
		 */
		bool DumpBuffer (unsigned char *buffer, size_t size);
		bool DumpBuffer (unsigned char *buffer);

		void WritePPM (char *filename, int n = -1);
		int ReadPPM (char *filename);

		void Details (void);
		void Stain (unsigned int times = 1, unsigned int dp = 3);

		unsigned char Normalize (unsigned char v, 
								 unsigned char m, 
								 unsigned char M,
								 unsigned char ll = 0,
								 unsigned char ul = MIMAGE_ON);

		double Normalize (double v, 
								 double m, 
								 double M,
								 double ll = 0,
								 double ul = MIMAGE_ON);
	
		bool TransformLinear (double m, double c);
		
		bool Filter (int id);
		bool RequestFilter (int id, short int *Sv, short int *Sh, double *f);

		bool HistogramG (void);
	
		bool Equalize (void);
		void Process1 (void);
		void Process (unsigned int mode);

		void Test (void);
		bool SearchCircles (int r, int dr, unsigned int *Cx, unsigned int *Cy);
		
		/* PushMem () and PopMem ()
		 * are unstable and should
		 * not be used!
		 */
		bool PushMem (void);
		bool PopMem (void);

	private:
		/*
		 * _GetPixel (x, y) at channel c
		 */
		unsigned char _GetPixel (unsigned int x,
									   unsigned int y, 
									   unsigned int c);
		
		/*
		 * _SetPixel (x, y) at channel c, value v
		 */
		inline void _SetPixel (unsigned int x,
							  unsigned int y, 
							  unsigned int c, 
							  unsigned char v);
	
		inline void _ComputeSize (mImage *image);


		bool _ReadHeaderPPM (FILE *file);
		char * _ReadChunkPPM (FILE *file);

		bool _DataCopy (mImage *to, mImage *from);
		bool _DataNew (mImage *image, 
				      unsigned int w, 
					  unsigned int h, 
					  unsigned int bpp = MIMAGE_G, 
					  bool binary = false);
	//private:
	public:
		mImage *_image;
		mImage *_imageBackup;
		mHistG *_histG;
		
		mHistG *_histRGB_R;
		mHistG *_histRGB_G;
		mHistG *_histRGB_B;
};


mObjectIMG::mObjectIMG ()
{
	SetName ("mObjectIMG");
	
#ifdef VERBOSE
	PrintName ();
	printf ("Starting %s\n", GetName ());
#endif
	_image = NULL;
	_histG = NULL;
}


void mObjectIMG::New (unsigned int w, unsigned int h, unsigned int bpp)
{
	assert (bpp == MIMAGE_G || bpp == MIMAGE_RGB);

	if (_image != NULL)
		delete _image;

	_image = new mImage;
	
	_image->w = w > 0 ? w : MOBJECTIMG_STD_WIDTH;
	_image->h = h > 0 ? h : MOBJECTIMG_STD_HEIGHT;
	_image->bpp = bpp;
	_image->binary = false;
	_image->data = NULL;

	_ComputeSize (_image);
#ifdef VERBOSE
	PrintName ();
	printf ("Image details: %dx%d %dbpp\n", _image->w, _image->h, 8*_image->bpp);
	PrintName ();
	printf ("Allocating %.3f KB (%d)\n", (float)_image->bytes/1024, _image->bytes);
#endif
	
	_image->data = (unsigned char *)malloc (_image->bytes);

	assert (_image->data != NULL);

	memset (_image->data, MIMAGE_OFF, _image->bytes);
}

mObjectIMG::~mObjectIMG ()
{
#ifdef VERBOSE
	PrintName ();
	printf ("Destroying %s\n", GetName ());
#endif
	if (_histG != NULL)
		delete _histG;
	
	if (_image != NULL)
		delete _image;
}

inline void mObjectIMG::_ComputeSize (mImage *image)
{
	assert (image->w > 0 && image->h > 0 && image->bpp > 0);
	image->bytes = image->w * image->h * image->bpp;
}

inline bool mObjectIMG::CheckPixel (unsigned int x, unsigned int y, unsigned int c)
{
	if (_image->bpp == MIMAGE_G)
		return (x < _image->w) && (y < _image->h) && (c == 0);
	else if (_image->bpp == MIMAGE_RGB)
		return (x < _image->w) && (y < _image->h) && (c >= 0) && (c < MIMAGE_RGB);
	else
		return false;
}

unsigned char mObjectIMG::_GetPixel (unsigned int x, unsigned int y, unsigned int c)
{
	assert (CheckPixel (x, y, c));
	return	(_image)->data [_image->bpp * (x  + y * _image->w) + c];
}
		
unsigned char mObjectIMG::GetPixelG (unsigned int x, unsigned int y)
{
	return _GetPixel (x, y, MCH_G);
}


inline void mObjectIMG::_SetPixel (unsigned int x, unsigned int y, unsigned int c, unsigned char v)
{
	assert (CheckPixel (x, y, c));
	(_image)->data [_image->bpp*(x + y * _image->w) + c] = v;
}


void mObjectIMG::SetPixelG (unsigned int x, unsigned int y, unsigned char V)
{
	_SetPixel (x, y, MCH_G, V);
}


void mObjectIMG::SetPixelRGB (unsigned int x, unsigned int y, unsigned char R, unsigned char G, unsigned char B)
{
	_SetPixel (x, y, MCH_RGB_R, R);
	_SetPixel (x, y, MCH_RGB_G, G);
	_SetPixel (x, y, MCH_RGB_B, B);
}


void mObjectIMG::SetPixelBGR (unsigned int x, unsigned int y, unsigned char B, unsigned char G, unsigned char R)
{
	_SetPixel (x, y, MCH_RGB_R, R);
	_SetPixel (x, y, MCH_RGB_G, G);
	_SetPixel (x, y, MCH_RGB_B, B);
}


void mObjectIMG::WritePPM (char *filename, int n)
{
    FILE *file;
    char filenameT [1024];



	if (_image->bpp == MIMAGE_G)
		if (n < 0)
			sprintf (filenameT, "%s.pgm", filename);
		else
			sprintf (filenameT, "%s_%.6d.pgm", filename, n);
	else if (_image->bpp == MIMAGE_RGB)
		if (n < 0)
			sprintf (filenameT, "%s.ppm", filename);
		else
			sprintf (filenameT, "%s_%.6d.ppm", filename, n);
	
	file = fopen (filenameT, "wb");
	assert (file != NULL);
    
	int result = 0;
	if (_image->bpp == MIMAGE_G)
	{
		fprintf (file, "P5\n%d %d 255\n", _image->w, _image->h);
		result = fwrite (_image->data, _image->w*_image->h, 1, file);
	}
	else if (_image->bpp == MIMAGE_RGB)
	{
		fprintf (file, "P6\n%d %d 255\n", _image->w, _image->h);
		result = fwrite (_image->data, _image->w*_image->h*3, 1, file);
	}
	assert (result >= 0);

    fclose (file);
}


bool mObjectIMG::DumpBuffer (unsigned char *buffer, size_t size)
{
	if (_image->data == NULL || _image == NULL)
		return false;

	return (memcpy (_image->data, buffer, size) != NULL);
}


bool mObjectIMG::DumpBuffer (unsigned char *buffer)
{
	if (_image->data == NULL || _image == NULL)
		return false;

	return (memcpy (_image->data, buffer, _image->bytes) != NULL);
}


bool mObjectIMG::Resize (unsigned int w, unsigned int h)
{
	if (w <= 0 || h <= 0)
		return false;

	mImage *imageT = new mImage;
	if (imageT == NULL)
		return false;

	if (!_DataNew (imageT, w, h, _image->bpp, _image->binary))
		return false;
	
	unsigned int pnew = 0;
	unsigned int pold = 0;

	for (unsigned int x = 0; x < w; x++)
		for (unsigned int y = 0; y < h; y++)
		{
			pnew = (x + y*w) * _image->bpp;
			pold = (x*_image->w/w + y*_image->h/h*_image->w) * _image->bpp;
			for (unsigned int c = 0; c < _image->bpp; c++)
				imageT->data [pnew + c] = _image->data [pold + c];
		}
	
	delete _image;
	_image = imageT;

	return true;
}


bool mObjectIMG::Grey (void)
{
	if (!IsRGB () || !IsAllocated ())
		return false;
	
	mImage *imageT = new mImage;
	if (imageT == NULL)
		return false;

	if (!_DataNew (imageT, _image->w, _image->h, MIMAGE_G, _image->binary))
		return false;

	double *temp = new double [_image->w * _image->h];
	
	for (unsigned int p = 0; p < imageT->w*imageT->h; p++)
	{
		temp [p] += (double)_image->data [p*3 + 0];
		temp [p] += (double)_image->data [p*3 + 1];
		temp [p] += (double)_image->data [p*3 + 2];
		temp [p] /= 3;
	}
	
	double m = temp [0];
	double M = temp [0];
	for (unsigned int i = 0; i < _image->w * _image->h; i++)
	{
		if (temp [i] > M) 
			M = temp [i];
		if (temp [i] < m) 
			m = temp [i];
	}

	/*	
	for (unsigned int i = 0; i < _image->w * _image->h; i++)
		temp [i] = (temp [i] - m) * (MIMAGE_ON) / (M - m);
	*/

	for (unsigned int p = 0; p < imageT->w*imageT->h; p++)
		imageT->data [p] = (unsigned char)ceil (temp [p]);
	
	if (_image != NULL)
		delete _image;
	_image = imageT;

	return true;
}


bool mObjectIMG::Crop (unsigned int x0, unsigned int y0, unsigned int w, unsigned int h)
{
	unsigned int tw = 0;
	unsigned int th = 0;

	if (w == 0)
		tw = _image->w - 1;
	else
		tw = w;

	if (h == 0)
		th = _image->h - 1;
	else
		th = h;


	if (!CheckPixel (x0 + tw, y0 + th, MCH_G))
		return false;
	if (!IsAllocated ())
		return false;

	
	mImage *imageT = new mImage;
	if (imageT == NULL)
		return false;

	if (!_DataNew (imageT, tw, th, _image->bpp, _image->binary))
		return false;

	for (unsigned int x = x0; x < x0 + tw; x++)
		for (unsigned int y = y0; y < y0 + th; y++)
			for (unsigned int c = 0; c < imageT->bpp; c++)
				imageT->data [imageT->bpp * ((x - x0)  + (y - y0) * imageT->w) + c] =
				_image->data [_image->bpp * (x  + y * _image->w) + c];

	delete _image;
	_image = imageT;

	return true;
}


bool mObjectIMG::Negative (void)
{
	if (!IsAllocated ())
		return false;
	
	for (unsigned int i = 0; i < _image->w*_image->h*_image->bpp; i++)
		_image->data [i] = MIMAGE_ON - _image->data [i];

	return true;
}


bool mObjectIMG::Threshold (unsigned int th)
{
	if (!IsAllocated ())
		return false;
	

	if (IsGrey ())
		for (unsigned int x = 0; x < _image->w; x++)
			for (unsigned int y = 0; y < _image->h; y++)
				if (GetPixelG (x, y) > th)
					SetPixelG (x, y, MIMAGE_ON);
				else
					SetPixelG (x, y, MIMAGE_OFF);
	else if (IsRGB ())
		for (unsigned int x = 0; x < _image->w; x++)
			for (unsigned int y = 0; y < _image->h; y++)
				for (unsigned int c = MCH_RGB_R; c < MCH_RGB_B; c++)
					if (_GetPixel (x, y, c) > th)
						_SetPixel (x, y, c, MIMAGE_ON);
					else
						_SetPixel (x, y, c, MIMAGE_OFF);

	_image->binary = true;
		
	return true;
}
		

bool mObjectIMG::Threshold (unsigned int thL, unsigned int thH, unsigned int dp)
{
	if (!IsAllocated () || !IsGrey ())
		return false;

	cout << GetPixelG (10, 10);
	for (unsigned int x = 0; x < _image->w; x++)
		for (unsigned int y = 0; y < _image->h; y++)
			if (GetPixelG (x, y) >= thH)
				SetPixelG (x, y, MIMAGE_ON);
			else if (GetPixelG (x, y) < thH)
				SetPixelG (x, y, 0);

	unsigned int add = 1;
	while (add > 0)
	{
		add = 0;
		for (unsigned int x = 0; x < _image->w; x++)
			for (unsigned int y = 0; y < _image->h; y++)
				if (GetPixelG (x, y) >= thL && GetPixelG (x, y) <= thH)
					if (x > dp && x < _image->w && y > dp && y < _image->h)
						if (GetPixelG (x,      y + dp) == MIMAGE_ON ||
							GetPixelG (x,      y - dp) == MIMAGE_ON ||
							GetPixelG (x + dp, y     ) == MIMAGE_ON ||
							GetPixelG (x - dp, y     ) == MIMAGE_ON)
						{
							SetPixelG (x, y, MIMAGE_ON);
							add++;
						}
	}

	for (unsigned int x = 0; x < _image->w; x++)
		for (unsigned int y = 0; y < _image->h; y++)
			if (GetPixelG (x, y) < MIMAGE_ON)
				SetPixelG (x, y, MIMAGE_OFF);
	
	_image->binary = true;
	
	return true;
}


/* Original code by:
 *  2003 Patrick Reynolds <reynolds@cs.duke.edu>
 */
char * mObjectIMG::_ReadChunkPPM (FILE *file)
{
	static char buffer [200] = "\0";
	static char *p = buffer;
	char *q, *tmp;
	if (!*p) 
	{
		do
			fgets (buffer, 200, file);
		while (*buffer == '#');
		p = buffer;
	}
	while (*p == ' ' || *p == '\t') 
		p++;
	q = p;
	
	while (*q != ' ' && *q != '\t' && *q != '\n' && *q != '\0') 
		q++;
	tmp = p;
	
	if (*q)     
		p = q + 1;
	else
		p = q;
	*q = '\0';

	return tmp;
}


/* Original code by:
 *  2003 Patrick Reynolds <reynolds@cs.duke.edu>
 */
bool mObjectIMG::_ReadHeaderPPM (FILE *file) 
{
	char *buffer;
	int ign;

	buffer = _ReadChunkPPM (file);
	if (!strcmp("P5", buffer))
		_image->bpp = MIMAGE_G;
	else if (!strcmp("P6", buffer))
		_image->bpp = MIMAGE_RGB;
	else
		return false;


	buffer = _ReadChunkPPM (file); 
	_image->w = atoi (buffer);

	buffer = _ReadChunkPPM (file);
	_image->h = atoi (buffer);

	buffer = _ReadChunkPPM (file);   
	ign = atoi (buffer);

	if (ign != MIMAGE_ON)
		return false;

	return true;
}


/* Original code by:
 *  2003 Patrick Reynolds <reynolds@cs.duke.edu>
 */
int mObjectIMG::ReadPPM (char *filename) 
{
	FILE *file = new FILE;
	int n = 0;
	unsigned int bytesread = 0;

	if (_image != NULL)
		delete _image;
	_image = new mImage;

	file = fopen (filename, "rb+");
	assert (file != NULL);
	
	if (!file || !_image) 
		return -1;


	if (!_ReadHeaderPPM (file))
	{
		fprintf (stderr, "Input does not appear to be in PPM format\n");
		return -1;
	}

	_ComputeSize (_image);
	_image->data = (unsigned char *)malloc (_image->bytes);

	while (bytesread < _image->bytes) 
	{
		n = fread (_image->data + bytesread, 1, _image->bytes - bytesread, file);
		if (n <= 0) 
			return -1;
		bytesread += n;
	}

	return 0;
}


void mObjectIMG::Details (void)
{
	PrintName ();
	printf ("Image details:\n");

	if (!IsAllocated ())
		printf ("  Error: image not allocated\n");
	else
	{
		printf ("  Resolution  %dx%d px\n", _image->w, _image->h);
		printf ("  Depth       %d bpp\n", 	_image->bpp);
		printf ("  Memory      %d KB\n",  	(int)(_image->bytes/1024));
		if (IsGrey ())
		{
		printf ("  Max value   %d\n", 		Max ());
		printf ("  Min value   %d\n", 		Min ());
		printf ("  Mean value  %d\n", 		Mean ());
		}
	}
}

		
bool mObjectIMG::Dilate (unsigned char on, unsigned char off)
{
	if (!IsAllocated () || !IsGrey ())
		return false;

	unsigned int dp = 1;

	for (unsigned int x = dp; x < _image->w - dp; x++)
		for (unsigned int y = dp; y < _image->h - dp; y++)
			if (GetPixelG (x, y + dp) == on ||
					GetPixelG (x, y - dp) == on ||
					GetPixelG (x + dp, y) == on || 
					GetPixelG (x - dp, y) == on)
				if (GetPixelG (x, y) == off)
					SetPixelG (x, y, 127);
	
	for (unsigned int x = 0; x < _image->w; x++)
		for (unsigned int y = 0; y < _image->h; y++)
			if (GetPixelG (x, y) == 127)
				SetPixelG (x, y, on);

	return true;
}


bool mObjectIMG::Erode (unsigned char on, unsigned char off)
{
	if (!IsAllocated () || !IsGrey ())
		return false;

	unsigned int dp = 1;

	for (unsigned int x = dp; x < _image->w - dp; x++)
		for (unsigned int y = dp; y < _image->h - dp; y++)
			if (GetPixelG (x, y + dp) >= 127 &&
					GetPixelG (x, y - dp) >= 127 &&
					GetPixelG (x + dp, y) >= 127 && 
					GetPixelG (x - dp, y) >= 127)
				if (GetPixelG (x, y) == on)
					SetPixelG (x, y, 127);
	
	for (unsigned int x = 0; x < _image->w; x++)
		for (unsigned int y = 0; y < _image->h; y++)
			if (GetPixelG (x, y) == 127)
				SetPixelG (x, y, on);
			else			
				SetPixelG (x, y, off);
	return true;
}


bool mObjectIMG::Closing (unsigned int times)
{
	if (times == 0)
		return false;
	
	unsigned int t = 0;
	while (t++ < times)
		if (!Dilate ())
			return false;

	t = 0;
	while (t++ < times)
		if (!Erode ())
			return false;
	
	return true;
}


bool mObjectIMG::Opening (unsigned int times)
{
	if (times == 0)
		return false;
	
	unsigned int t = 0;
	while (t++ < times)
		if (!Erode ())
			return false;

	t = 0;
	while (t++ < times)
		if (!Dilate ())
			return false;
	
	return true;
}


unsigned char mObjectIMG::Max (void)
{
	assert (IsAllocated () == true || IsGrey () == true);

	unsigned char max = MIMAGE_WHITE;

	for (unsigned int p = 0; p < _image->w * _image->h * _image->bpp; p++)
		if (_image->data [p] > max)
			max = _image->data [p]; 

	return max;

}


unsigned char mObjectIMG::Min (void)
{
	assert (IsAllocated () == true || IsGrey () == true);

	unsigned char min = MIMAGE_BLACK;

	for (unsigned int p = 0; p < _image->w * _image->h * _image->bpp; p++)
		if (_image->data [p] < min)
			min = _image->data [p]; 
	
	return min;
}


unsigned char mObjectIMG::Mean (void)
{
	assert (IsAllocated () == true || IsGrey () == true);

	unsigned long mean = 0;
	for (unsigned int p = 0; p < _image->w * _image->h * _image->bpp; p++)
		mean += _image->data [p];

	return (unsigned char)(mean / (_image->w * _image->h * _image->bpp));
}


bool mObjectIMG::RequestFilter (int id, short int *Sv, short int *Sh, double *f)
{
	if (Sv == NULL || Sh == NULL)
		return false;

	switch (id)
	{
		case MFILTER_SOBEL:
			*f = 1;
			// <-----> //
			Sv [0] = -1;
			Sv [1] =  0;
			Sv [2] =  1;
			Sv [3] = -2;
			Sv [4] =  0;
			Sv [5] =  2;
			Sv [6] = -1;
			Sv [7] =  0;
			Sv [8] =  1;
			// <-----> //
			Sh [0] =  1;
			Sh [1] =  2;
			Sh [2] =  1;
			Sh [3] =  0;
			Sh [4] =  0;
			Sh [5] =  0;
			Sh [6] = -1;
			Sh [7] = -2;
			Sh [8] = -1;
			break;
		case MFILTER_LAPLACE_4:
			*f = 1;
			// <-----> //
			Sv [0] =  0;
			Sv [1] = -1;
			Sv [2] =  0;
			Sv [3] = -1;
			Sv [4] =  4;
			Sv [5] = -1;
			Sv [6] =  0;
			Sv [7] = -1;
			Sv [8] =  0;
			break;
		case MFILTER_LAPLACE_8:
			*f = 1/8;
			// <-----> //
			Sv [0] =  0;
			Sv [1] = -1;
			Sv [2] =  0;
			Sv [3] = -1;
			Sv [4] =  8;
			Sv [5] = -1;
			Sv [6] =  0;
			Sv [7] = -1;
			Sv [8] =  0;
			break;
		case MFILTER_NOISE_9:
			*f = 1/9;
			// <-----> //
			Sv [0] =  1;
			Sv [1] =  1;
			Sv [2] =  1;
			Sv [3] =  1;
			Sv [4] =  1;
			Sv [5] =  1;
			Sv [6] =  1;
			Sv [7] =  1;
			Sv [8] =  1;
			break;
		case MFILTER_NOISE_10:
			*f = 1/10;
			// <-----> //
			Sv [0] =  1;
			Sv [1] =  1;
			Sv [2] =  1;
			Sv [3] =  1;
			Sv [4] =  2;
			Sv [5] =  1;
			Sv [6] =  1;
			Sv [7] =  1;
			Sv [8] =  1;
			break;
		case MFILTER_NOISE_16:
			*f = 1/16;
			// <-----> //
			Sv [0] =  1;
			Sv [1] =  2;
			Sv [2] =  1;
			Sv [3] =  2;
			Sv [4] =  4;
			Sv [5] =  2;
			Sv [6] =  1;
			Sv [7] =  2;
			Sv [8] =  1;
			break;
		case MFILTER_M1:
			*f = 1;
			// <-----> //
			Sv [0] = -1;
			Sv [1] = -1;
			Sv [2] = -1;
			Sv [3] = -1;
			Sv [4] =  8;
			Sv [5] = -1;
			Sv [6] = -1;
			Sv [7] = -1;
			Sv [8] = -1;
		default: 
			return false;
	}
	return true;
}


bool mObjectIMG::Filter (int id)
{
	if (!IsAllocated () || !IsGrey ())
		return false;

	/* Allocate Horiz. (Sh) and Vert. (Sv)
	 * filters.
	 */
	short int *Sv = new short int [MFILTER_SIZE];
	short int *Sh = new short int [MFILTER_SIZE];
	short int *St = NULL;
	double f = 0;

	/* Fill filter values 
	 * (if the filter requested does not exist, 
	 * exit).
	 */
	bool result = RequestFilter (id, Sv, Sh, &f);
	f = 10;
	if (!result)
		return false;

	double *Fv = new double [_image->w * _image->h];
	double *Fh = new double [_image->w * _image->h];
	double *Ft = NULL;
	double *Fr = new double [_image->w * _image->h];

	int m = 0;
	
	const unsigned int dp = 1;

	double Fm;
	double FM;

	bool loop = true;
	St = Sv;
	Ft = Fv;

	while (loop)
	{	
		for (unsigned int i = 0; i < 9; i++)
			m += St [i];

		if (m == 0)
			m = 1;

		for (unsigned int x = dp; x < _image->w - dp; x++)
			for (unsigned int y = dp; y < _image->h - dp; y++)
			{
				Ft [x + y * _image->w] = 
					(double)(( St [0] * GetPixelG (x - 1, y - 1)
							 + St [1] * GetPixelG (x - 1, y    )
							 + St [2] * GetPixelG (x - 1, y + 1)
							 + St [3] * GetPixelG (x    , y - 1)
							 + St [4] * GetPixelG (x    , y    )
							 + St [5] * GetPixelG (x    , y + 1)
							 + St [6] * GetPixelG (x + 1, y - 1)
							 + St [7] * GetPixelG (x + 1, y    )
							 + St [8] * GetPixelG (x + 1, y + 1)) * f / m);
			}
		
		if (id > 0)
			if (St == Sv && Ft == Fv)
			{		
				loop = true;
				St = Sh;
				Ft = Fh;
			}
			else if (St == Sh && Ft == Fh)
				loop = false;
		else 
			loop = false;
	}
	

	if (id > 0)
		for (unsigned int i = 0; i < _image->w * _image->h; i++)
			Fr [i] = fabs (Fv [i]) + fabs (Fh [i]);
	else
		for (unsigned int i = 0; i < _image->w * _image->h; i++)
			Fr [i] = fabs (Fv [i]);

	Fm = Fr [0];
	FM = Fr [0];
	for (unsigned int i = 0; i < _image->w * _image->h; i++)
	{
		if (Fr [i] > FM) 
			FM = Fr [i];
		if (Fr [i] < FM) 
			Fm = Fr [i];
	}

	for (unsigned int i = 0; i < _image->w * _image->h; i++)
		Fr [i] = (Fr [i] - Fm) * (MIMAGE_ON) / (FM -Fm);

	for (unsigned int x = dp; x < _image->w - dp; x++)
		for (unsigned int y = dp; y < _image->h - dp; y++)
			SetPixelG (x, y, (unsigned char)Fr [x + y * _image->w]);

	delete Fh;
	delete Fv;
	delete Fr;
	delete Sv;
	delete Sh;

	return true;
}


unsigned char mObjectIMG::Normalize (unsigned char v, unsigned char m, unsigned char M, unsigned char ll, unsigned char ul)
{
	return (v - m) * (ul - ll) / (M -m);
}


double mObjectIMG::Normalize (double v, double m, double M, double ll, double ul)
{
	return (v - m) * (ul - ll) / (M -m);
}

bool mObjectIMG::TransformLinear (double m, double c)
{
	for (unsigned int x = 0; x < _image->w; x++)
		for (unsigned int y = 0; y < _image->h; y++)
			SetPixelG (x, y, (unsigned char)floor(m * GetPixelG (x, y) + c));
	return true;
}


void mObjectIMG::Stain (unsigned int times, unsigned int dp)
{
	unsigned int t = 0;

	while (t++ < times)
		for (unsigned int x = 0; x < _image->w; x++)
			for (unsigned int y = 0; y < _image->h; y++)
			{
				int add = 0;
				for (unsigned int d = 0; d < dp; d++)
				{
					if (CheckPixel (x + d, y+ d, 0))
						if (GetPixelG (x + d, y + d) == MIMAGE_ON)
							add++;
				}
				if (add < (int)(dp - 1))
					SetPixelG (x, y, MIMAGE_OFF);
			}
}


bool mObjectIMG::_DataCopy (mImage *to, mImage *from)
{
	if (to == NULL || from == NULL)
		return false;

	/*
	if (to != NULL && from == NULL)
		from = _image;
*/
	to->w = from->w;
	to->h = from->h;
	to->bpp = from->bpp;
	to->binary = from->binary;
	to->bytes = from->bytes;

	return memcpy (to->data, from->data, from->bytes);	
}


bool mObjectIMG::_DataNew (mImage *image, unsigned int w, unsigned int h, unsigned int bpp, bool binary)
{
	if (image == NULL)
		return false;
	
	if (w <= 0 || h <= 0)
		return false;
	
	if (bpp != MIMAGE_G && bpp != MIMAGE_RGB)
		return false;
	
	image->w = w;
	image->h = h;
	image->bpp = bpp;
	image->binary = binary;
	image->bytes = w * h * bpp;
	image->data = NULL;
	image->data = (unsigned char *)malloc (image->bytes);
	if (image->data == NULL)
		return false;

	return true;
}


bool mObjectIMG::HistogramG (void)
{
	if (!IsGrey () || !IsAllocated ())
		return false;

	if (_histG == NULL)
		_histG = new mHistG;

	memset (_histG, 0, MHIST_SIZE);

	for (unsigned int x = 0; x < _image->w; x++)
		for (unsigned int y = 0; y < _image->h; y++)
			_histG->data [GetPixelG (x, y)]++;	

	_histG->min = _histG->data [0];
	_histG->max = _histG->data [0];
	
	double mean = 0;
	for (unsigned int h = 0; h < MHIST_SIZE; h++)
	{
		if (_histG->data [h] > _histG->max)
			_histG->max = _histG->data [h];
		if (_histG->data [h] < _histG->min)
			_histG->min = _histG->data [h];
		mean += _histG->data [h];
	}	
	_histG->mean = (unsigned char)floor(mean / MHIST_SIZE);
	return true;
}


void mObjectIMG::Process (unsigned int mode)
{
	ReadPPM ("Test/tongue.ppm");
	switch (mode)
	{
		case 0:
			Grey ();
			HistogramG ();
			Negative ();
			TransformLinear (1, -127 - _histG->mean/2);
			Negative ();
			Threshold (_histG->mean);
			WritePPM ("Test/Processing_1.ppm");
			break;
		case 1:
			Grey ();
			HistogramG ();
			Negative ();
			TransformLinear (1.5, -127);
			Negative ();
			Threshold (_histG->mean);
			WritePPM ("Test/Processing_2.ppm");
			break;
	}
}


bool mObjectIMG::Equalize (void)
{
	if (!IsGrey () || !IsAllocated ())
		return false;

	if (!HistogramG ())
		return false;

	TransformLinear (1 / (_histG->max - _histG->min), _histG->min / (_histG->max - _histG->min));
	//Negative ();
	//TransformLinear (-1.5, -127);
	//Negative ();
	Threshold (_histG->mean);
	cout << (int)_histG->mean << endl;
	
	return true;
}


void mObjectIMG::Process1 (void)
{
	/* Tongue sample processing
	 * Simply extract tongue shape
	 */

	int n = 0;

	ReadPPM ("Test/tongue.ppm");
	
	Grey ();
	WritePPM ("Test/p1/_tongue", n++);
	
	Filter (MFILTER_NOISE_16);
	WritePPM ("Test/p1/_tongue", n++);

	Threshold (85);
	WritePPM ("Test/p1/_tongue", n++);
	
	Stain ();
	Filter (MFILTER_NOISE_9);
	Threshold (MIMAGE_ON - 1);
	WritePPM ("Test/p1/_tongue", n++);
}


bool mObjectIMG::SearchCircles (int r, int dr, unsigned int *Cx, unsigned int *Cy)
{
	double *votes = new double [_image->w * _image->h];
	memset (votes, 1, _image->w * _image->h * sizeof (double));

	double score = 0;

	for (unsigned int x = 0; x < _image->w; x++)
		for (unsigned int y = 0; y < _image->h; y++)
			for (int dx = -(r + dr); dx < (r + dr); dx++)
				for (int dy = -(r + dr); dy < (r + dr); dy++)
					if (x + dx > 0 && x + dx < _image->w)
						if (y + dy > 0 && y + dy < _image->h)
							if (GetPixelG (x + dx, y + dy) == MIMAGE_ON)
							{
								score = 1e6*sqrt((x + abs(dx) - r)^2 + (y + abs(dy) - r)^2);
								if (GetPixelG (x, y) == MIMAGE_OFF)
									score *= 10;
								votes [y * _image->w + x] *= score;
							}
	
	double max = votes [0];
	double min = votes [0];
	double mean = 0;
	for (unsigned int i = 0; i < _image->w *_image->h; i++)
	{
		if (votes [i] > max)
			max = votes [i];
		if (votes [i] < min)
			min = votes [i];
		mean += votes [i];
	}
	mean /= _image->w * _image->h;

	cout << mean << endl;
	cout << min << endl;
	cout << max << endl;

	for (unsigned int i = 0; i < _image->w *_image->h; i++)
		votes [i] = Normalize (votes [i], min, max, MIMAGE_OFF, MIMAGE_ON);

	unsigned int centers = 0;
	unsigned int xAcc = 0;
	unsigned int yAcc = 0;

	for (unsigned int x = 0; x < _image->w; x++)
		for (unsigned int y = 0; y < _image->h; y++)
			if (votes [y * _image->w + x] == MIMAGE_ON)
			{
#ifdef DEBUG
				cout << "x = " << x << "\t";
				cout << "y = " << y << "\n";
#endif
				xAcc += x;
				yAcc += y;
				centers++;
			}
		/*
		   else
		   SetPixelG (x, y, MIMAGE_OFF);
		*/
	
				

	if (centers >= 1)
	{
		*Cx = (unsigned int) (xAcc / centers);
		*Cy = (unsigned int) (yAcc / centers);
	}
	else
	{
		*Cx = 0;
		*Cy = 0;
	}


	SetPixelG (*Cx    , *Cy    , 127);
	SetPixelG (*Cx + 1, *Cy    , 127);
	SetPixelG (*Cx    , *Cy    , 127);
	SetPixelG (*Cx - 1, *Cy    , 127);
	SetPixelG (*Cx    , *Cy + 1, 127);
	SetPixelG (*Cx    , *Cy - 1, 127);

	PrintName ();
	printf ("Circles detected:\n");
	printf ("  Radius            %d (+-%d) px\n", r, dr);
	printf ("  Detected centers  %d\n", centers);
	printf ("  Winner centre     (%d, %d)\n", *Cx, *Cy);

	if (centers >= 1)
		return true;
	else
		return false;
}


bool mObjectIMG::PushMem (void)
{
	if (!IsAllocated ())
		return false;
		
	if (_imageBackup != NULL)
		delete _imageBackup;

	_imageBackup = new mImage;
	return _DataCopy (_imageBackup, _image);

}


bool mObjectIMG::PopMem (void)
{
	if (!IsAllocated () || _imageBackup == NULL)
		return false;

	if (_image != NULL)
		delete _image;

	_image = new mImage;
	return _DataCopy (_image, _imageBackup);
}

#endif
































