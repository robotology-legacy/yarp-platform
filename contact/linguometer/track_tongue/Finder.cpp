// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#include "Finder.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


int Finder::main(int argc, char *argv[]) {
  Property p;
  p.fromCommand(argc,argv);
  if (p.check("file")) {
    p.fromConfigFile(p.check("file",Value("config.ini")).asString());
    // allow overrides if needed
    p.fromCommand(argc,argv,true,false);
  }

  BufferedPort<ImageOf<PixelRgb> > outPort;
  outPort.open("/tongue");
  Network::connect("/tongue",p.check("viewer",Value("/view")).asString());

  // command line options should describe something that can provide
  // audio (and/or video)

  PolyDriver device;
  bool ok = device.open(p);
  if (!ok) {
    printf("Could not open device.\n");
    printf("Usage example:\n");
    printf("./track_tongue --file config.ini\n");
    exit(1);
  }
  IFrameGrabberImage *imageSource;
  device.view(imageSource);
  if (imageSource==NULL) {
    printf("Device does not supply images.\n");
    device.close();
    exit(1);
  }

  PolyDriver saver;
  if (p.check("saver")) {
    ConstString v = p.findGroup("saver").toString();
    printf("saver is %s\n", v.c_str());
    saver.open(v);
  }
  IFrameWriterImage *imageSink;
  saver.view(imageSink);


  setVerbose(p.check("verbose"));
  ImageOf<PixelRgb> image;
  while (imageSource->getImage(image)) {

    ImageOf<PixelRgb>& out = outPort.prepare();
    process(image,out);
    
    if (imageSink!=NULL) {
      imageSink->putImage(out);
    }

    outPort.write();

  }

  device.close();

  return 0;
}
