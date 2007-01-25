#include "mObjectScroller.h"
/*
 * Threads are not gonna be used 
 *#include <pthread.h>
 */

#define BUFFER_SIZE		1024

#define STATUS_NONE		0
#define STATUS_PAUSE	1
#define STATUS_BACK		2
#define STATUS_QUIT		3


void PrintName (void)
{
	printf ("[mWords] ");
}


bool LoadWords (char *filename, vector<string> *v, unsigned int bsize)
{
	if (filename == NULL || v == NULL || bsize <= 0)
		return false;

	PrintName ();
	printf ("Loading word list (file %s):\n", filename);
	
	char *buffer = new char [bsize];
  	ifstream fin (filename);	
	int i = 0;

	printf ("%6s %27s\n", "Index", "Word");
	while (1)
	{
		memset (buffer, '\0', bsize);
		fin.getline (buffer, bsize, '\n');
		if (fin.eof ())
		{
			printf ("  ----------------------------------------\n");
			break;
		}

		if (buffer [0] != '#' && 
			buffer [0] != ' ' && 
			buffer [0] != '\0' && 
			buffer [0] != '\n')
		{
			printf ("  %.4d %27s\n", i++, buffer);
			string str (buffer);
			v->push_back (str);
		}
	}
	fin.close ();

	return true;
}


unsigned int GetPosition (unsigned int ll, unsigned int ul)
{
	return (unsigned int)floor(rand () * ((double)ul - (double)ll) / (RAND_MAX)); 
}


void CreatePattern (int **sequence, int *ssize, unsigned int size, unsigned int reps)
{
	PrintName ();
	printf ("Creating Random Pattern: %d events, %d repetitions\n", size, reps);
	srand( time(NULL) );

	int idx = 0;

	int *assigned = new int [size];
	assigned    = (int *)malloc (sizeof (int) * size);
	(*sequence) = (int *)malloc (sizeof (int) * size * reps);

	memset (assigned,     0, sizeof (int) * size);
	memset ((*sequence), -1, sizeof (int) * size * reps);

	for (unsigned int i = 0; i < size; i++)
	{
		while (assigned [i] < (int)reps)
		{
			idx = GetPosition (0, size * reps);
			if ((*sequence) [idx] == -1)
			{
				(*sequence) [idx] = i;
				assigned [i]++;
			}
		}
	}
	*ssize = reps * size;
}

int main (int argc, char *argv[])
{
	char *filename = "words/words_testD.dat";
	bool gofs = 1;
	mObjectScroller *scroller;

	if (gofs)
	{
		scroller = new mObjectScroller ();
		scroller->SetFont  ("fonts/Vera.ttf", 200);
	}
	else
	{
		scroller = new mObjectScroller (640, 480);
		scroller->SetFont  ("fonts/Vera.ttf", 50);
	}

	scroller->SetTimes (30, 5, true);
	scroller->InitVideo ();

	vector<int> *positions = new vector<int>; 
	vector<int> *rejected = new vector<int>; 
	vector<string> *words = new vector<string>; 

	LoadWords (filename, words, BUFFER_SIZE);
	
	int *sequence = NULL;
	int seqsize = 0;
	CreatePattern (&sequence, &seqsize, words->size (), 1);

	PrintName ();
	printf ("Starting scroller...\n");

	bool log = true;
	bool run = true;
	int idx = 0;
	int msg = 0;
	
	for (unsigned int i = 0; i < (unsigned int)seqsize; i++)
		positions->push_back (sequence [i]);

	delete sequence;

	scroller->SetFontColor (0xff, 0x7c, 0xd0);
	msg = scroller->RenderWordIO ("Ready to go.");
	for (unsigned int j = 1; ; j++)
	{
		printf ("%6s %6s %20s\n", "Event", "Index", "Word");
		for (unsigned int i = 0; i < positions->size () && run;)
		{
			idx = positions->at (i);

			if (log)
			{
				scroller->SetTimes (30, 150);
				scroller->RenderWord (" ");
				scroller->SetTimes (30, 5);
				printf ("%6.4d %6.4d %20s\n", i, idx, words->at (idx).c_str ());
			}

			scroller->SetFontColor (0xff, 0xff, 0xff);
			msg = scroller->RenderWordIO (words->at (idx).c_str ());

			switch (msg)
			{ 
				case MOS_MSG_OK:
					log = true; 
					i++;
					break; 
				case MOS_MSG_REJECT:
					rejected->push_back (idx);
					log = true;	
					i++;
					break; 
				case MOS_MSG_QUIT:
					run = false;
					break; 
				default:
					log = false;
					break;
			}
		}
	
		if (!run || rejected->empty ())
			break;
		positions->clear (); 
		*positions = *rejected;
		rejected->clear ();
	} 
	scroller->SetFontColor (0xff, 0x7c, 0xd0);
	msg = scroller->RenderWordIO ("Done!");

	delete scroller;
	return 0;
}
