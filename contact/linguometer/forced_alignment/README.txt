
Hello good evening and welcome to a big bunch of scripts.

The purpose of these scripts is to run forced alignment on a corpus
of italian speech collected in Lecce by Michele Tavella

-------------------------------------------------------------------------

DEPENDENCIES:

Developed and tested on Debian GNU/Linux.

It relies on the following resources.  They are a bit fiddly to install, 
but worth it in the end:

From the CMUSPHINX team, we get a speech recognizer:
  sphinx3
  SphinxTrain's wave2feat (needed by sphinx3 but not included)

From FESTIVAL, we get an Italian phonetic dictionary:
  apt-get install festlex-ifd (on debian)

Assorted tools:
  octave (could be replaced by matlab with a small bit of work)
  sox (for resampling audo to 16k)

-------------------------------------------------------------------------

STARTING POINT:

Read and edit top of Makefile
Type "make"

-------------------------------------------------------------------------

Note: have to check if accents are ever used in the Lecce corpus.
It so may need something the "recode" package, e.g:
  echo [something with accents] | recode utf8..lat1 | festival ...
