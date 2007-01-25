#include "mObjectDV.h"
#include "mObjectIMG.h"
#include "mContainer.h"
#include "mObjectEncoder.h"

//#define GO_ALL

#ifdef GO_ALL
#	define GO_RAW
#	define GO_DV1
#	define GO_DV2
#	define GO_WAV
#endif


void welcome (void)
{
	printf ("%s v%s by %s (%s)\n\n", PROJECT_MPDV, VERSION_MPDV, AUCTOR, MAIL);
}


void help (void)
{
	printf ("Usage:\n  mprocessdv --stream file --ao file --vo path\n");
	printf ("             [--from hh:mm:ss] [--to hh:mm:ss]\n\n");

	printf ("Options:\n");
	printf ("  --stream     DV Type-1, DV Type-2 or DV RAW stream\n");
	printf ("  --ao         Audio Output\n");
	printf ("  --vo         Video Output\n");
	printf ("\n");
	
	printf ("Audio Output options (--ao):\n");
	printf ("  null         Disable\n");
	printf ("  file.pcm     Export PCM stream(s) in plain ASCII\n");
	printf ("  file.ext     If \"ext\" is a format supported by FFMPEG,\n");
	printf ("               mProcessDV will export the audio streams\n");
	printf ("               using the specified format.\n");
	printf ("\n");
	
	printf ("Video Output options (--vo):\n");
	printf ("  null         Disable\n");
	printf ("  path     	Export frames in PPM/PGM format\n");
	printf ("\n");
	
	printf ("Seek Options (--from and --to):\n");
	printf ("  --to only    This switch limits audio/video\n");
	printf ("               decoding/encoding to the selected\n");
	printf ("               interval (beginning --> hh:mm:ss).\n");
	printf ("  --from only  This switch limits audio/video\n");
	printf ("               decoding/encoding to the selected\n");
	printf ("               interval (hh:mm:ss --> end).\n");
	printf ("\n");

	printf ("Other options:\n");
	printf ("  --version    display version number\n");
	printf ("  --about      display terms of use\n");
	printf ("\n");

	printf ("Notes:\n");
	printf ("  mProcessDV is a frontend for:\n");
	printf ("    1. mObjectDV.h\n");
	printf ("    2. mObjectPCM.h\n");
	printf ("    3. mObjectIMG.h\n");
	printf ("    4. mObjectEncoder.h\n");
	printf ("  The code is far away from being complete and\n");
	printf ("  has been written for the CONTACT Project\n");
	printf ("  setup (www.liralab.it/contact).\n");
	printf ("\n");
}


void user_exit (void)
{
	char c;
	cout << "\nPress \"q\" to quit ";
	while (cin >> c)
		if (c == 'q')
			break;
	cout << endl;	
}


void build (void)
{
}


void test (void)
{
	char *file = "/home/michele/ContactData/Experiments/experiment_0009/US/stream.dv";

	mObjectDV *movieDV = new mObjectDV ();
	movieDV->SetName ("movieDV");

	movieDV->SetStream (file);
	movieDV->SetupAudio (1, 0);

	movieDV->DecodeInit ();
	movieDV->DecodeFrom (0, 4, 0);
	movieDV->DecodeTo   (0, 6, 0);
	movieDV->DecodeAudio ();
	
	unsigned int samples;
	int16_t *bufferL = NULL;
	int16_t *bufferR = NULL;
	movieDV->streamAudio->ExportBuffers (&bufferL, &bufferR, samples);

	int16_t sat = (int16_t)(pow (2, 16 - 1)/2);
	bool st0 = true;
	bool st1 = true;

	for (unsigned int s = 0; s < samples; s++)
	{
		if (st0 && st1)
		{
			st0 = false;
			st1 = false;
			printf ("Looking for new sweep!\n");
		}

		if (bufferL [s] == -sat && !st0 && !st1)
		{
			printf ("--> Sweep t0 at: %d\n", s);
			st0 = true;
		}
		else if (bufferL [s] == +sat && !st1)
		{
			printf ("--> Sweep t1 at: %d\n", s);
			st1 = true;
		}
	}

	movieDV->PrintDetails ();	
	delete movieDV;
}


void decode_audio (char *stream, char *file,
				   int fh, int fm, int fs,
		           int th, int tm, int ts)
{
	mObjectDV *streamDV = new mObjectDV ();

	streamDV->SetStream (stream);
	streamDV->SetAudioOut (file);
	streamDV->SetupAudio (1, 1);
	
	streamDV->DecodeInit ();

	if (fh > 0 || fm > 0 || fs > 0)
		streamDV->DecodeFrom (fh, fm, fs);

	if (th > 0 || tm > 0 || ts > 0) 
		streamDV->DecodeTo (th, tm, ts);

	streamDV->DecodeAudio ();

	streamDV->PrintDetails ();	

	delete streamDV;
}


void decode_video (char *stream, char *folder,
				   int fh, int fm, int fs,
		           int th, int tm, int ts)
{
	mObjectDV *streamDV = new mObjectDV ();

	streamDV->SetStream (stream);
	streamDV->SetVideoOut (folder);
	streamDV->SetupVideo (1, 1);
	
	streamDV->DecodeInit ();
	
	if (fh > 0 || fm > 0 || fs > 0)
		streamDV->DecodeFrom (fh, fm, fs);

	if (th > 0 || tm > 0 || ts > 0) 
		streamDV->DecodeTo (th, tm, ts);

	streamDV->DecodeVideo ();
	
	streamDV->PrintDetails ();	

	delete streamDV;
}

void decode_audiovideo (char *stream, char *file, char *folder,
				   int fh, int fm, int fs,
		           int th, int tm, int ts)
{
	mObjectDV *streamDV = new mObjectDV ();

	streamDV->SetStream (stream);
	streamDV->SetAudioOut (file);
	streamDV->SetVideoOut (folder);
	streamDV->SetupAudio (1, 1);
	streamDV->SetupVideo (1, 1);

	streamDV->DecodeInit ();
	
	if (fh > 0 || fm > 0 || fs > 0)
		streamDV->DecodeFrom (fh, fm, fs);

	if (th > 0 || tm > 0 || ts > 0) 
		streamDV->DecodeTo (th, tm, ts);
	
	streamDV->DecodeAudio ();
	streamDV->DecodeVideo ();

	streamDV->PrintDetails ();	

	delete streamDV;
}


int main (int argc, char *argv[])
{
	char *seekF = NULL;
	int fh = -1;
	int fm = -1;
	int fs = -1;

	char *seekT = NULL;
	int th = -1;
	int tm = -1;
	int ts = -1;
	

	switch (argc)
	{
		case 2:
			if (strcmp (argv [1], "--test") == 0)
			{
				welcome ();
				test ();
			}
			if (strcmp (argv [1], "--help") == 0)
			{
				welcome ();
				help ();
			}
			if (strcmp (argv [1], "--about") == 0)
			{
				welcome ();
  				printf("%s", DISCLAIMER);
			}
			if (strcmp (argv [1], "--version") == 0)
			{
				welcome ();
			}
			break;
		case 11:
			if (strcmp (argv [7], "--from") == 0 && strcmp (argv [9], "--to") == 0)
			{
				seekF = argv [8];
				seekT = argv [10];
			}
		case 9:
			if (strcmp (argv [7], "--from") == 0)
				seekF = argv [8];
			else if (strcmp (argv [7], "--to") == 0)
				seekT = argv [8];
		case 7:
			if (strcmp (argv [1], "--stream") == 0 && 
				strcmp (argv [3], "--ao") == 0 && 
				strcmp (argv [5], "--vo") == 0)
			{
				welcome ();
				
				char *st = argv [2];
				char *ao = argv [4];
				char *vo = argv [6];

				bool ad = true;
				bool vd = true;

				printf ("Input/Output details:\n");
				printf ("  Stream in: %s\n", st);
				printf ("  Audio out: %s\n", ao);
				printf ("  Video out: %s\n", vo);
				printf ("Seeking details:\n");
				printf ("  Seek From: %s\n", seekF != NULL ? seekF : "disabled");
				printf ("  Seek To:   %s\n", seekT != NULL ? seekT : "disabled");
	
				int rF = 0;
				if (seekF != NULL)
				{
					rF = sscanf (seekF, "%d:%d:%d", &fh, &fm, &fs);
					if (rF != 3)
					{
						fh = -1;
						fm = -1;
						fs = -1;
					}
				}
				
				int rT = 0;
				if (seekT != NULL)
				{
					rT = sscanf (seekT, "%d:%d:%d", &th, &tm, &ts);
					if (rT != 3)
					{
						th = -1;
						tm = -1;
						ts = -1;
					}
				}
				
				if (strcmp (ao, "null") == 0 || strcmp (ao, "NULL") == 0)
					ad = false;
				
				if (strcmp (vo, "null") == 0 || strcmp (vo, "NULL") == 0)
					vd = false;

				printf ("Exporting:\n");
				// Audio + Video 
				if (ad == true && vd == true)
				{
					printf ("  Audio and Video\n");
					decode_audiovideo (st, ao, vo, fh, fm, fs, th, tm, ts);	
				}

				// Audio Only
				if (ad == true && vd == false)
				{
					printf ("  Audio only\n");
					decode_audio (st, ao, fh, fm, fs, th, tm, ts);	
				}

				// Video Only
				if (ad == false && vd == true)
				{
					printf ("  Video only\n");
					decode_video (st, vo, fh, fm, fs, th, tm, ts);	
				}

				//user_exit ();
			}
			break;
		default:
			welcome ();
			help ();
			break;
	}
	return 0;
}


/*
 * Discontinued
 */
void PelletDetect (void)
{
	/*
	mObjectIMG *imageC = NULL;
	unsigned int x_small = 0;
	unsigned int y_small = 0;
	unsigned int x_big = 0;
	unsigned int y_big = 0;
	unsigned int i = 0;
	*/

	/*
	i = 0;
	imageC = new mObjectIMG;
	imageC->ReadPPM ("Test/PelletDetect/white.ppm");
	imageC->Grey ();
	imageC->WritePPM ("Test/PelletDetect/_white", i++);
	imageC->Negative ();
	imageC->Filter (MFILTER_SOBEL);
	imageC->Threshold (100);
	x_small = 0;
	y_small = 0;
	imageC->SearchCircles (3, 1, &x_small, &y_small);
	imageC->WritePPM ("Test/PelletDetect/_white", i++);
	delete imageC;
	
	imageC = new mObjectIMG;
	imageC->ReadPPM ("Test/PelletDetect/white.ppm");
	imageC->Grey ();
	imageC->Negative ();
	imageC->Filter (MFILTER_SOBEL);
	imageC->Threshold (100);
	x_big = 0;
	y_big = 0;
	imageC->SearchCircles (9, 2, &x_big, &y_big);
	imageC->WritePPM ("Test/PelletDetect/_white", i++);
	delete imageC;

	i = 0;
	x_big = 0;
	y_big = 0;
	
	imageC = new mObjectIMG;
	imageC->ReadPPM ("Test/PelletDetect/whitenoise.ppm");
	imageC->Grey ();
	imageC->WritePPM ("Test/PelletDetect/_whitenoise", i++);
	imageC->Crop (0, 100, 0, 200);
	imageC->Negative ();
	imageC->Filter (MFILTER_LAPLACE_8);
	//imageC->Filter (MFILTER_SOBEL);
	imageC->Threshold (100);
	imageC->SearchCircles (3, 2, &x_big, &y_big);
	imageC->WritePPM ("Test/PelletDetect/_whitenoise", i++);
	delete imageC;

	i = 0;
	imageC = new mObjectIMG;
	imageC->ReadPPM ("Test/PelletDetect/whitenoise.ppm");
	imageC->WritePPM ("Test/PelletDetect/_filta", i++);
	imageC->Grey ();
	//imageC->Crop (0, 80, 0, 200);
	//imageC->Filter (MFILTER_LAPLACE_8);
	imageC->Filter (MFILTER_SOBEL);
	//imageC->Filter (MFILTER_M1);
	//imageC->Threshold (80);
	imageC->SearchCircles (3, 1, &x_big, &y_big);
	imageC->WritePPM ("Test/PelletDetect/_filta", i++);
	delete imageC;
	*/


}


void align (void)
{
	int n = 0;
	
	// <--- Next ---> //
	
	n = 0;	
	mObjectIMG *imageC = new mObjectIMG;
	imageC->ReadPPM ("Test/circles2.ppm");
	imageC->Grey ();
	imageC->Negative ();
	imageC->Filter (MFILTER_SOBEL);
	imageC->Threshold (100);
	unsigned int x_small = 0;
	unsigned int y_small = 0;
	imageC->SearchCircles (3, 1, &x_small, &y_small);
	imageC->WritePPM ("Test/_circle_small");
	delete imageC;
	
	imageC = new mObjectIMG;
	imageC->ReadPPM ("Test/circles2.ppm");
	imageC->Grey ();
	imageC->Negative ();
	imageC->Filter (MFILTER_SOBEL);
	imageC->Threshold (100);
	unsigned int x_big = 0;
	unsigned int y_big = 0;
	imageC->SearchCircles (9, 2, &x_big, &y_big);
	imageC->WritePPM ("Test/_circle_big");



	return;
	// <--- Next ---> //
	
	n = 0;	
	mObjectIMG *imageP1 = new mObjectIMG;
	imageP1->Process (0);	
	delete imageP1;

	mObjectIMG *imageP2 = new mObjectIMG;
	imageP2->Process (1);	
	delete imageP2;
	
	// <--- Next ---> //
	
}
