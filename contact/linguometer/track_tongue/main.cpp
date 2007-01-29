
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#include <yarp/sig/GlutOrientation.h>

#include <deque>

class Point { 
public:
  double x, y;
  
  Point(double x, double y) : x(x), y(y) {}
};

using namespace std;

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace yarp::dev;

int main(int argc, char *argv[]) {
  Property p;
  p.fromCommand(argc,argv);
  if (p.check("file")) {
    p.fromConfigFile(p.check("file",Value("config.ini")).asString());
  }

  BufferedPort<ImageOf<PixelRgb> > outPort;
  outPort.open("/tongue");
  Network::connect("/tongue",p.check("hit",Value("/view")).asString());

  // command line options should describe something that can provide
  // audio (and/or video)

  PolyDriver device;
  bool ok = device.open(p);
  if (!ok) {
    printf("Could not open device.\n");
    printf("Usage example:\n");
    printf("./track_tongue --device ffmpeg_grabber --source /scratch/tongue.avi --noloop\n");
    printf("can also add --saver ffmpeg_writer to save output\n");

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


  double xglob = 0, yglob = 0;
  bool globSet = false;

  int ct = 0;
  double peak = 0;
  double t = 0;
  ImageOf<PixelRgb> image, part;
  ImageOf<PixelFloat> dx, dy, mag, work;
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
    work.resize(part);
    work.zero();
    IMGFOR(part,x,y) {
      double ddx = dx(x,y);
      double ddy = dy(x,y);
      if (mag(x,y)>0.8) {
	for (int g=-3; g<=3; g++) {
	  for (int f=-10; f<10; f++) {
	    work.safePixel((int)(x+f*ddx+g*ddy),(int)(y+f*ddy-g*ddx))++;
	  }
	}
      }
    }
    double top = 0.01;
    int tx = -1, ty = -1;
    IMGFOR(work,x,y) {
      if (work(x,y)>top && y<work.height()*0.75 && 
	  y>work.height()*0.2) {
	top = work(x,y);
	tx = x;
	ty = y;
      }
    }
    IMGFOR(work,x,y) {
      work(x,y) = 254*work(x,y)/top;
      if (work(x,y)>255) {
	work(x,y) = 255;
      }
    }
    out.copy(part);
    ImageOf<PixelFloat> mask;
    mask.resize(out);
    mask.zero();
    for (int vx=-2; vx<=2; vx++) {
      for (int j=-1; j<=1; j+=2) {
	double rx = tx+vx;
	double ry = ty;
	double pdx = j;
	double pdy = 0;
	double odx = 0;
	double ody = 0;
	for (int i=0; i<50; i++) {      
	  int dd = 5;
	  for (int xx=-dd;xx<=dd; xx++) {
	    for (int yy=-dd;yy<=dd; yy++) {
	      PixelFloat& pix = mask.safePixel((int)rx+xx,(int)ry+yy);
	      pix = 1;
	    }
	  }
	  double ndx = dx.safePixel((int)rx,(int)ry);
	  double ndy = dy.safePixel((int)rx,(int)ry);
	  if (pdx*ndx+pdy*ndy<0) {
	    ndx *= -1;
	    ndy *= -1;
	  }
	  if (fabs(odx)+fabs(ody)<0.001) {
	    odx = ndx;
	    ody = ndy;
	  }
	  if (odx*ndx+ody*ndy<-0.5) {
	    break;
	  }
	  rx += ndx;
	  ry += ndy;
	  pdx = ndx;
	  pdy = ndy;
	}
      }
    }
    /*
    IMGFOR(out,x,y) {
      out(x,y) = part(x,y);
      if (mask(x,y)<0.5) {
	PixelRgb& pix = out(x,y);
	pix.g = pix.b = 0;
      }
      image(x+xmin,y+ymin) = out(x,y);
      //image(x+xmin,y+ymin).r = work(x,y);
      //image(x+xmin,y+ymin).g = work(x,y);
      //image(x+xmin,y+ymin).b = work(x,y);
    }
    */
    deque<Point> tongue;
    {
      for (int s=-1; s<=1; s+=2) {
	double bx = tx;
	double by = ty;
	double dx = s;
	double dy = 0;
	double ct = 0;
	double vbase = work.safePixel((int)bx,(int)by);
      
	for (int i=0; i<80; i++) {
	  bx = bx+dx;
	  by = by+dy;
	  double best = 0;
	  double nx = bx;
	  double ny = by;
	  for (int dd=-4; dd<=4; dd++) {
	    double v = work.safePixel((int)bx,(int)by+dd);
	    if (v>best) {
	      best = v;
	      ny = by+dd;
	    }
	  }
	  if (best<vbase*0.25) {
	    ct++;
	    if (ct>5) {
	      break;
	    }
	  } else {
	    ct = 0;
	  }
	  bx = nx;
	  by = ny;
	  //addCircle(image,PixelRgb(0,0,255),bx+xmin,by+ymin,3);
	  if (s==1) {
	    tongue.push_back(Point(bx+xmin,by+ymin));
	  } else {
	    tongue.push_front(Point(bx+xmin,by+ymin));
	  }
	}
      }
    }

    ImageOf<PixelFloat> locs, locs2;
    int tops = tongue.size();
    locs.resize(2,tops);
    locs2 = locs;

    int at = 0;
    for (deque<Point>::iterator it=tongue.begin(); it!=tongue.end(); it++) {
      Point p = *it;
      locs(0,at) = p.x;
      locs(1,at) = p.y;
      //printf("(%g,%g)\n", p.x, p.y);
      at++;
    }
    //printf("begin\n");
    for (int k=0; k<10; k++) {
      locs2 = locs;
      for (int i=1; i<tops-1; i++) {
	double x = (0.5*locs(0,i-1)+locs(0,i)+0.5*locs(0,i+1))/2;
	double y = (0.5*locs(1,i-1)+locs(1,i)+0.5*locs(1,i+1))/2;
	double xx = locs(0,i);
	locs2(0,i) = x;
	locs2(1,i) = y;
      }
      locs = locs2;
    }
    at = 0;
    for (deque<Point>::iterator it=tongue.begin(); it!=tongue.end(); it++) {
      Point& p = *it;
      p.x = locs(0,at);
      p.y = locs(1,at);
      at++;
    }

    
    //for (int r=-1; r<=1; r+=2) {
    for (int r=0; r<=0; r+=2) {
      int del = 10*r;
      for (deque<Point>::iterator it=tongue.begin(); it!=tongue.end(); it++) {
	Point p = *it;
	if (int(p.x/7)%2==0) {
	  addCircle(image,PixelRgb(0,0,255),p.x,p.y+del,3);
	}
      }
      
      for (deque<Point>::iterator it=tongue.begin(); it!=tongue.end(); it++) {
	Point p = *it;
	if (int(p.x/7)%2==0) {
	  addCircle(image,PixelRgb(255,0,0),p.x,p.y+del,1);
	}
      }
    }

    if (!globSet) {
      xglob = tx+xmin;
      yglob = ty+ymin;
    } else {
      // need to do some tracking
    }

    addRectangleOutline(image,PixelRgb(0,255,0),image.width()/2,
			image.height()/2, out.width()/2, out.height()/2);
    out = image;
    
    if (imageSink!=NULL) {
      imageSink->putImage(out);
    }

    //addCircle(out,PixelRgb(0,255,0),xglob,yglob,5);
    outPort.write();

  }

  device.close();

  return 0;
}
