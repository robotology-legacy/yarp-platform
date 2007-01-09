
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <yarp/dev/all.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main(int argc, char *argv[]) {
  Property p;
  p.fromCommand(argc,argv);


  // command line options should describe something that can provide
  // audio (and/or video)

  PolyDriver device;
  bool ok = device.open(p);
  if (!ok) {
    printf("Could not open device.\n");
    printf("Usage example:\n");
    printf("./click_segment --device ffmpeg_grabber --source click.wav --nodelay --noloop\n");

    exit(1);
  }
  IAudioGrabberSound *audioSource;
  device.view(audioSource);
  if (audioSource==NULL) {
    printf("Device does not supply audio.\n");
    device.close();
    exit(1);
  }

  Sound sound;
  int ct = 0;
  double peak = 0;
  double t = 0;
  while (audioSource->getSound(sound)) {
    ct++;
    
    int samples = sound.getSamples();
    int channels = sound.getChannels();
    t += ((double)samples)/sound.getFrequency();
    
    // we assume the channel we want is channel 0
    int channel = 0;

    double total = 0;
    for (int i=0; i<samples; i++) {
      total += fabs(sound.get(i));
    }
    if (total>peak) {
      peak = total;
    }
    int cut = false;
    if (total<peak*0.01) {
      cut = true;
      peak = 0;
    }
    if (0) printf("%06d  %s  total %g\n", ct, 
		  cut?"cut":"   ",
		  total);
    if (cut) {
      printf("cut at audio frame %d (time=%g)\n",
	     ct, t);
    }
    double frac = 0.9;
    peak = peak*frac + total*(1-frac);
  }

  device.close();

  return 0;
}
