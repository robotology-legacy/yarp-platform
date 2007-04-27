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
LIBS =  -lm -lz -lavformat -lavcodec -lz
CFLAGS = -I../../include/ -g

include ../../config/Makefile.cflags
include ../../config/Makefile.in
