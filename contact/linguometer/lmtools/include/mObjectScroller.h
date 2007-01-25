/* Coded by M. Tavella
 * email michele@liralab.it
 * web   http://mt.homelinux.org 
 */

#ifndef MOBJECTSCROLLER_H
#define MOBJECTSCROLLER_H

#include "mInclude.h"
#include "mClass.h"

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define	MOS_MSG_NONE		-2
#define	MOS_MSG_REFRESH		-1
#define	MOS_MSG_QUIT		0
#define	MOS_MSG_PAUSE		1
#define	MOS_MSG_PREV		2
#define	MOS_MSG_NEXT		3
#define	MOS_MSG_OK			4
#define	MOS_MSG_KO			5
#define	MOS_MSG_SHUTUP		6
#define	MOS_MSG_KEEPGOING	7
#define MOS_MSG_REJECT		8



class mObjectScroller : public mClass
{
	public:
		mObjectScroller (int w = 0, int h = 0);
		~mObjectScroller ();

		bool SetFont 	(char *file, int size);
		void SetFontColor (Uint8 r, Uint8 g, Uint8 b);
		bool SetTimes 	(int fps, int delay, bool verbose = false);
		void InitVideo 	(void);
		void RenderWord (const char *word);
		int RenderWordIO (const char *word);
		
	private:
		void CreateWord (const char *word);

	public:

	private:
		int  _screenW;
		int  _screenH;
		bool _screenFS;
		
		int  _wordW;
		int  _wordH;

		TTF_Font *_font;
		char *_fontFile;
		int  _fontSize;
		SDL_Color _fontColor;
	
		SDL_Surface *_surfaceScreen;	
		SDL_Surface **_surfaceLetter;
		SDL_Rect 	*_rectLetter;
	
		int _delay;
		int _fps;
			
		Uint32 _black;


};


mObjectScroller::mObjectScroller (int w, int h)
{
	SetName ("mObjectScroller");
	PrintName ();
	printf ("Starting: ");

	if (w == 0 || h == 0)
	{
		printf ("Full Screen\n");
		_screenFS = true;
	}
	else 
	{
		_screenW = w;
		_screenH = h;

		printf ("Windowed (%d x %d)\n", _screenW, _screenH);
		_screenFS = false;
	}
}


mObjectScroller::~mObjectScroller ()
{
	PrintName ();
	printf ("Quitting\n");
}


bool mObjectScroller::SetFont (char *file, int size)
{
	if (file == NULL || size <= 0)
		return false;

	_fontFile = new char [1024];
	memset (_fontFile, 0, 1024);
	strcpy (_fontFile, file);

	_fontSize = size;

	PrintName ();
	printf ("TTF Font: %s (%d px)\n", _fontFile, _fontSize);

	return true;
}


bool mObjectScroller::SetTimes (int fps, int delay, bool verbose)
{
	if (fps <= 0 || delay <= 0)
		return false;

	_fps = fps;
	_delay = delay;

	if (verbose)
	{
		PrintName ();
		printf ("Times: %d fps, delay %d ms\n", _fps, _delay);
	}

	return true;
}


void mObjectScroller::CreateWord (const char *word)
{
	char buffer [2];

	TTF_SizeText (_font, word, &_wordW, &_wordH);
	_surfaceLetter = (SDL_Surface **)malloc(sizeof(SDL_Surface *) * strlen(word));
	_rectLetter = (SDL_Rect *)malloc(sizeof(SDL_Rect) * strlen(word));
	
	for (unsigned int i = 0; i < strlen (word); i++)
	{
		buffer [0] = word [i];
		buffer [1] = '\0';
		_surfaceLetter [i] = TTF_RenderText_Blended (_font, buffer, _fontColor);
		_rectLetter [i].w = _surfaceLetter [i]->w;
		_rectLetter [i].h = _surfaceLetter [i]->h;
	}
}


void mObjectScroller::InitVideo (void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		PrintName ();
		printf ("Error: SDL_Init (%s)\n", SDL_GetError());
		exit (-1);
	}
	
	atexit(SDL_Quit);

	int vmode = SDL_HWSURFACE|SDL_DOUBLEBUF;
	if (_screenFS)
		vmode = vmode|SDL_FULLSCREEN;
	
	if (_screenFS)
	{
		const SDL_VideoInfo *info = SDL_GetVideoInfo ();
		_screenW = info->current_w;
		_screenH = info->current_h;
	}

	if ((_surfaceScreen = SDL_SetVideoMode (_screenW, _screenH, 0, vmode)) == NULL)
	{
		PrintName ();
		printf ("Error: SDL_VideoMode (%s)\n", SDL_GetError());
		exit (-1);
	}
	
	if (TTF_Init())
	{
		PrintName ();
		printf ("Error: TTF_Init (%s)", TTF_GetError());
		exit (-1);
	}
	
	if((_font = TTF_OpenFont (_fontFile, _fontSize)) == NULL)
	{
		PrintName ();
		printf ("Error: TTF_OpenFont (%s)", TTF_GetError());
		exit (-1);
	}
	
	_black = SDL_MapRGB (_surfaceScreen->format, 0x00, 0x00, 0x00);
	
	_fontColor.r = 0xff;
	_fontColor.g = 0xff;
	_fontColor.b = 0xff;

	SDL_WM_SetCaption("CONTACT Words Scroller", "mScroller");
	SDL_ShowCursor(0);

}


void mObjectScroller::SetFontColor (Uint8 r, Uint8 g, Uint8 b)
{
	_fontColor.r = r;
	_fontColor.g = g;
	_fontColor.b = b;
}

void mObjectScroller::RenderWord (const char *word)
{
	CreateWord (word);

	Sint16 x = (_screenW - _wordW) / 2;
	Sint16 y = (_screenH - _wordH) / 2;

	for (unsigned int i = 0; i < strlen (word); i++)
	{
		_rectLetter [i].x = x;
		_rectLetter [i].y = y;
		x += _rectLetter [i].w;
		SDL_BlitSurface (_surfaceLetter [i], NULL, _surfaceScreen, &_rectLetter [i]);
	}
	
	int delay = 0;
	while (delay <= _delay)
	{
		SDL_Delay (1000/_fps);
		delay += 1000/_fps;
		SDL_Flip (_surfaceScreen);
	}

	for (unsigned int i = 0; i < strlen (word); i++)
		SDL_FillRect (_surfaceScreen, &_rectLetter [i], _black);
	
	for (unsigned int i = 0; i < strlen (word); i++)
		SDL_FreeSurface(_surfaceLetter [i]);
}

int mObjectScroller::RenderWordIO (const char *word)
{
	int keypress = 0;
	int msg = 0;
	SDL_Event event;

	while(!keypress) 
	{
		RenderWord (word);
		while(SDL_PollEvent(&event)) 
		{    
			if( event.type == SDL_KEYDOWN ) 
			{
				keypress = 1;
				switch (event.key.keysym.sym)
				{ 
					case SDLK_UP: 
						msg = MOS_MSG_PREV; 
						break; 
					case SDLK_DOWN: 
						msg = MOS_MSG_NEXT;
						break; 
					case SDLK_LEFT: 
						msg = MOS_MSG_KO; 
						break; 
					case SDLK_RIGHT: 
						msg = MOS_MSG_OK;
						break; 
					case SDLK_x: 
						msg = MOS_MSG_REJECT;
						break; 
					case SDLK_q: 
						msg = MOS_MSG_QUIT;
						break; 
					case SDLK_z:	/* z for Zitto! 		*/ 
					case SDLK_s: 	/* s for Shut up! 		*/
						msg = MOS_MSG_SHUTUP;
						break; 
					case SDLK_p: 	/* p for Parla!			*/
					case SDLK_k: 	/* k for Keep going!	*/
						msg = MOS_MSG_KEEPGOING;
						break; 
					default:
						msg = MOS_MSG_NONE;
						break;
				}
			}
		}
	}
	return msg;
}
		
#endif
