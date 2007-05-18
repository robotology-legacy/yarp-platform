#-------------------
# Makefile Config.
#-------------------


MAIN = pdetect2
FLAGS=-DFFMPEG_DISTRO
FLAGS+=-DUSE_CONTACT_CACHE
FLAGS+=-DDEBUG_ALG
#FLAGS+=-DCONTACT_DATA=/home/michele/ContactData/
#FLAGS+=-DCONTACT_ROOT="/home/michele/ContactRoot/"
#FLAGS+=-DCONTACT_CACHE="/home/michele/ContactCache/"
LIBS =  -lm -lz -ldl -lavformat -lavcodec -lavutil
CFLAGS = -I../../include/ 

include ../../config/Makefile.cflags
include ../../config/Makefile.in
