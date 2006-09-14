
from: http://savannah.gnu.org/projects/gnuspeech

Requires debian packages (stable):
  gnustep gobjc gnustep-devel fftw-dev
and in debian unstable at time of writing also:
  libgnustep-base1.11-dev libgnustep-gui0.10-dev

Make sure to source the following before compiling anything:

. /usr/lib/GNUstep/System/Makefiles/GNUstep.sh

compile and install extensions in:
  Applications/Monet/Extensions/
using:
  make
  make install

compile in:
  Tools/softwareTRM
using:
  make

tube executable is:
  Tools/softwareTRM/obj/tube

