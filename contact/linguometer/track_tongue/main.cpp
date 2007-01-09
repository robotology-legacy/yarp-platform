
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#include <yarp/sig/GlutOrientation.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace yarp::dev;

int main(int argc, char *argv[]) {
  Property p;
  p.fromCommand(argc,argv);

  BufferedPort<ImageOf<PixelRgb> > outPort;
  outPort.open("/tongue");

  // command line options should describe something that can provide
  // audio (and/or video)

  PolyDriver device;
  bool ok = device.open(p);
  if (!ok) {
    printf("Could not open device.\n");
    printf("Usage example:\n");
    printf("./track_tongue --device ffmpeg_grabber --source /scratch/tongue.avi --noloop\n");

    exit(1);
  }
  IFrameGrabberImage *imageSource;
  device.view(imageSource);
  if (imageSource==NULL) {
    printf("Device does not supply images.\n");
    device.close();
    exit(1);
  }

  int ct = 0;
  double peak = 0;
  double t = 0;
  ImageOf<PixelRgb> image, part;
  ImageOf<PixelFloat> diff, dx, dy, mag;
  GlutOrientation orient("data/orient.txt");
  while (imageSource->getImage(image)) {
    ct++;

    int xmin = image.width()/4;
    int xmax = image.width()-xmin;
    int ymin = image.height()/4;
    int ymax = image.height()-ymin;

    part.resize(xmax-xmin+1,ymax-ymin+1);
    IMGFOR(part,x,y) {
      part(x,y) = image(x+xmin,y+ymin);
    }

    ImageOf<PixelRgb>& out = outPort.prepare();
    orient.Apply(part,out,dx,dy,mag);

    outPort.write();
  }

  device.close();

  return 0;
}
