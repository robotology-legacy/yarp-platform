#-------------------
# Makefile Config.
#-------------------


MAIN = pdetect
FLAGS=-DFFMPEG_DISTRO
FLAGS+=-DUSE_CONTACT_CACHE
FLAGS+=-DDEBUG_ALG
#FLAGS+=-DCONTACT_DATA=/home/michele/ContactData/
#FLAGS+=-DCONTACT_ROOT="/home/michele/ContactRoot/"
#FLAGS+=-DCONTACT_CACHE="/home/michele/ContactCache/"
LIBS =  -lm -lz -ldl -lavformat -lavcodec -lavutil
CFLAGS = -I../../include/ 

#FFMPEG_ROOT=../../lib/ffmpeg
#LDFLAGS=-L$(FFMPEG_ROOT)/
#LDFLAGS+=-L$(FFMPEG_ROOT)/libavformat
#LDFLAGS+=-L$(FFMPEG_ROOT)/libavcodec 
#LDFLAGS+=-L$(FFMPEG_ROOT)/libavutil
#LDFLAGS+=-Wl,--warn-common  -rdynamic -export-dynamic -Wl,--as-needed -Wl,-rpath-link,$(FFMPEG_ROOT)/libavcodec -Wl,-rpath-link,$(FFMPEG_ROOT)/libavformat -Wl,-rpath-link,$(FFMPEG_ROOT)/libavutil
#CFLAGS+=-I$(FFMPEG_ROOT)/libavformat -I$(FFMPEG_ROOT)/libavcodec -I$(FFMPEG_ROOT)/libavutil -I$(FFMPEG_ROOT)/

include ../../config/Makefile.cflags
include ../../config/Makefile.in
