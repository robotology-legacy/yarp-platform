// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>


#include <deque>
#include <fstream>

#include "TongueFinder.h"

#include "YARPViterbi.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace yarp::dev;


double dist(double x1,double y1,double x2,double y2) {
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}


void TongueFinder::processVersion1(ImageOf<PixelRgb>& image, 
                                   ImageOf<PixelRgb>& out) {

    int factor = 1;
    if (image.width()>400) {
        factor = 2;
    }

    if (isVerbose()) {
        saveImage(image,"00src");
    }

    int xmin = image.width()/4;
    int xmax = image.width()-xmin;
    int ymin = image.height()/4;
    int ymax = image.height()-ymin;


    // Extract important part of Toshiba image
    part.resize(xmax-xmin+1,ymax-ymin+1);
    IMGFOR(part,x,y) {
        part(x,y) = image(x+xmin,y+ymin);
    }

    // Get orientation within that region, then enhance it
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

    if (isVerbose()) {
        saveImage(out,"01dir");
    }
    ImageOf<PixelMono> tongueMask;
    tongueMask.resize(image);
    tongueMask.zero();


    // Gain control - scale based on most active part of regions
    double top = 0.01;
    int tx = -1, ty = -1;
    int x1mask = image.width(), x2mask = 0;
    int y1mask = image.height(), y2mask = 0;
    IMGFOR(work,x,y) {
        if (y<work.height()*0.75 && 
            y>work.height()*0.2) {
            if (work(x,y)>top) {
                top = work(x,y);
                tx = x;
                ty = y;
            }
            int xx = x+xmin;
            int yy = y+ymin;
            tongueMask(xx,yy) = 255;
            if (xx<x1mask) x1mask = xx;
            if (xx>x2mask) x2mask = xx;
            if (yy<y1mask) y1mask = yy;
            if (yy>y2mask) y2mask = yy;
        }
    }
    IMGFOR(work,x,y) {
        work(x,y) = 254*work(x,y)/top;
        if (work(x,y)>255) {
            work(x,y) = 255;
        }
    }


    if (isVerbose()) {
        ImageOf<PixelRgb> copy;
        copy.copy(work);
        saveImage(copy,"02enh");

        copy.copy(image);
        addRectangleOutline(copy,PixelRgb(0,255,0),image.width()/2,
                            image.height()/2, out.width()/2, out.height()/2);
        addRectangleOutline(copy,PixelRgb(255,0,0),
                            x1mask+(x2mask-x1mask+1)/2,
                            y1mask+(y2mask-y1mask+1)/2,
                            (x2mask-x1mask+1)/2,
                            (y2mask-y1mask+1)/2);
        saveImage(copy,"02msk");

        copy.copy(work);
        addCircleOutline(copy,PixelRgb(255,0,0),tx,ty,9*factor);
        addCircleOutline(copy,PixelRgb(255,0,0),tx,ty,8*factor);
        addCircleOutline(copy,PixelRgb(0,0,255),tx,ty,10*factor);
        addCircleOutline(copy,PixelRgb(0,0,255),tx,ty,7*factor);
        saveImage(copy,"02sed");
    }



    // Gather up a list of points that may be on the tongue
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
                if (best<vbase*0.3) {
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

    // Do some really brain-dead and inefficient smoothing
    ImageOf<PixelFloat> locs, locs2;
    int tops = tongue.size();
    locs.resize(2,tops);
    locs2 = locs;
    int at = 0;
    for (deque<Point>::iterator it=tongue.begin(); it!=tongue.end(); it++) {
        Point p = *it;
        locs(0,at) = p.x;
        locs(1,at) = p.y;
        at++;
    }
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

    for (int r=0; r<=0; r+=2) {
        int del = 10*r;
        for (deque<Point>::iterator it=tongue.begin(); it!=tongue.end(); it++) {
            Point p = *it;
            if (int(p.x/(7*factor))%2==0) {
                addCircle(image,PixelRgb(0,0,255),(int)p.x,(int)(p.y+del),3*factor);
            }
        }
      
        for (deque<Point>::iterator it=tongue.begin(); it!=tongue.end(); it++) {
            Point p = *it;
            if (int(p.x/(7*factor))%2==0) {
                addCircle(image,PixelRgb(255,0,0),(int)p.x,(int)(p.y+del),1*factor);
            }
        }
    }

    bool update = false;
    if (!globSet) {
        xglob = tx+xmin;
        yglob = ty+ymin;
        dxglob = dyglob = 0;
        globSet = true;
        update = true;
    } else {
        // need to do some tracking
        if (prevTongue.size()>0) {
            int dd = 5;
            int mx = 0, my = 0;
            double best = 0;
            for (int xx=-dd; xx<=dd; xx++) {
                for (int yy=-dd; yy<=dd; yy++) {
                    double val = 0;
                    for (deque<Point>::iterator it=prevTongue.begin(); 
                         it!=prevTongue.end(); 
                         it++) {
                        Point p = *it;
                        val += 
                            work.safePixel(int(p.x+xx-xmin),
                                           int(p.y+yy-ymin));
                    }
                    if (val>best) {
                        best = val;
                        mx = xx;
                        my = yy;
                    }
                }
            }
            if (fabs(mx)+fabs(my)>0) {
                dxglob += mx;
                dyglob += my;
                update = true;
            }
            double closest = 1e6;
            double sdxglob = dxglob;
            double sdyglob = dyglob;
            for (deque<Point>::iterator it=tongue.begin(); it!=tongue.end(); 
                 it++) {
                Point p = *it;
                //addCircle(image,PixelRgb(255,0,0),
                //    int(p.x+mx),int(p.y+my),1);
                double dx = p.x+mx-(xglob+dxglob);
                double dy = p.y+my-(yglob+dyglob);
                double dist = dx*dx+dy*dy;
                if (dist<closest) {
                    closest = dist;
                    sdxglob = p.x+mx-xglob;
                    sdyglob = p.y+my-yglob;
                }
            }
            if (closest>100) {
                dxglob = sdxglob;
                dyglob = sdyglob;
            }
        }
    }

    // leave tracking out, it is terrible so far
    //addCircle(image,PixelRgb(0,255,0),(int)(xglob+dxglob),
    //(int)(yglob+dyglob),5);

    if (update) {
        prevTongue = tongue;
    }
    
    addRectangleOutline(image,PixelRgb(0,255,0),image.width()/2,
                        image.height()/2, out.width()/2, out.height()/2);

    if (isVerbose()) {
        addCircleOutline(image,PixelRgb(255,0,0),xmin+tx,ymin+ty,9*factor);
        addCircleOutline(image,PixelRgb(255,0,0),xmin+tx,ymin+ty,8*factor);
        addCircleOutline(image,PixelRgb(0,0,255),xmin+tx,ymin+ty,10*factor);
        addCircleOutline(image,PixelRgb(0,0,255),xmin+tx,ymin+ty,7*factor);
    }

    out = image;

    if (isVerbose()) {
        saveImage(out,"03out");
    }


    ct++;
}


double comp(ImageOf<PixelFloat>& img, int x0, int y0,
            int x1, int y1) {
    return 1;
}

void TongueFinder::process(ImageOf<PixelRgb>& image, 
                           ImageOf<PixelRgb>& out) {

    static ImageOf<PixelFloat> prev;

    int factor = 1;
    if (image.width()>400) {
        factor = 2;
    }

    if (isVerbose()) {
        saveImage(image,"00src");
    }

    int xmin = image.width()/4;
    int xmax = image.width()-xmin;
    int ymin = image.height()/4;
    int ymax = image.height()-ymin;


    // Extract important part of Toshiba image
    part.resize(xmax-xmin+1,ymax-ymin+1);
    IMGFOR(work,x,y) {
        part(x,y) = image(x+xmin,y+ymin);
    }
    work.copy(part);
    if (prev.width()==0) {
        prev.resize(work);
        prev.zero();
    }

    // At back, tongue falls rapidly, and scan only captures small samples.
    // enhance those samples.
    IMGFOR(part,x,y) {
        if (x<work.width()/2) {
            PixelFloat& at = work.safePixel(x,y);
            PixelFloat& up = work.safePixel(x-5,y-5);
            if (at>up*1.2+50) {
                PixelFloat& at2 = work.safePixel(x+1,y);
                PixelFloat& up2 = work.safePixel(x-5+1,y-5);
                if (at2>up2*1.2+50) {
                    at = 255;
                }
            }
        }
    }

    out.copy(work);


    YARPViterbi path;
    path.SetSize(work.height()+2,part.width()+2);
    int PRE_STATE = work.height();
    int POST_STATE = work.height()+1;
    path.BeginTransitions();
    path.AddTransition(PRE_STATE,PRE_STATE,1);
    path.EndTransitions();
    float base = 30;
    for (int x=0; x<work.width(); x++) {
        path.BeginTransitions();
        path.AddTransition(PRE_STATE,PRE_STATE,0);
        path.AddTransition(POST_STATE,POST_STATE,0);
        for (int y=0; y<work.height(); y++) {
            float trans = 20;
            path.AddTransition(PRE_STATE,y,0);
            path.AddTransition(y,POST_STATE,0);

            double v = work(x,y);
            double local = base-v;
            local += work.safePixel(x,y-10);
            local += work.safePixel(x,y-20);
            if (v>30) {
                local -= prev.safePixel(x,y);
            }
            if (prev.safePixel(x,y)>128) {
                out(x,y).r = 80;
            }
            double ydev = ((double)y-work.height()/2)/(work.height()/2);
            double xdev = ((double)x-work.width()/2)/(work.width()/2);
            bool ok = true;

            // tongue just can't be here - but the chin often is
            if (ydev>0.75 || ydev<-0.75) {
                ok = false;
                out(x,y).b = 255;
            }
            if (fabs(xdev)<0.25&&ydev>0.5) {
                // in the central region, punish low values a bit
                out(x,y).g = 128;
                trans *= 10;
            }

            if (ok) {
                path.AddTransition(y,y,local +
                                   comp(work,x,y,x,y));
                for (int dy=-5; dy<=5; dy++) {
                    if (y!=0) {
                        if (y+dy>0 && y+dy<work.height()) {
                            path.AddTransition(y,y+dy,local +
                                               comp(work,x,y,x,y+dy) +
                                               trans*fabs(dy));
                        }
                    }
                }
            }
        }
        path.EndTransitions();
    }
    path.BeginTransitions();
    path.AddTransition(POST_STATE,POST_STATE,1);
    path.EndTransitions();

    path.CalculatePath();
    deque<Point> tongue;
    int py = -1;
    int px = -1;
    prev.zero();
    for (int x=1; x<=work.width(); x++) {
        int cx = x;
        int cy = path(x);
        if (cy<work.height()) {
            //addCircle(out,PixelRgb(255,0,0),cx,cy+10,3);
            //addCircle(out,PixelRgb(255,0,0),cx,cy-10,3);
            addCircle(prev,PixelFloat(255),cx,cy,30);
            if (px>=0) {
                double d = dist(px,py,cx,cy)+0.0000001;
                for (int i=0; i<=d; i++) {
                    double factor = i/d;
                    double ix = cx*factor+px*(1-factor);
                    double iy = cy*factor+py*(1-factor);
                    tongue.push_back(Point((int)ix,(int)iy));
                }
            }
            px = cx;
            py = cy;
        }
    }


    // Do some really brain-dead and inefficient smoothing
    ImageOf<PixelFloat> locs, locs2;
    int tops = tongue.size();
    locs.resize(2,tops);
    locs2 = locs;
    int at = 0;
    for (deque<Point>::iterator it=tongue.begin(); it!=tongue.end(); it++) {
        Point p = *it;
        locs(0,at) = p.x;
        locs(1,at) = p.y;
        at++;
    }
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

    for (int r=0; r<=0; r+=2) {
        int del = 10*r;
        for (deque<Point>::iterator it=tongue.begin(); it!=tongue.end(); it++) {
            Point p = *it;
            if (int(p.x/(7*factor))%2==0) {
                addCircle(out,PixelRgb(0,0,255),(int)p.x,(int)(p.y+del),3*factor);
            }
        }
      
        for (deque<Point>::iterator it=tongue.begin(); it!=tongue.end(); it++) {
            Point p = *it;
            if (int(p.x/(7*factor))%2==0) {
                addCircle(out,PixelRgb(255,0,0),(int)p.x,(int)(p.y+del),1*factor);
            }
        }
    }


    char buf[256];
    sprintf(buf,"path_%06d.txt",ct);
    ofstream fout(buf);
    int ppx = 0;
    int ppy = 0;
    for (deque<Point>::iterator it=tongue.begin(); it!=tongue.end(); it++) {
        Point p = *it;
        int xx = p.x;
        int yy = p.y;
        if (xx!=ppx) { // || yy!=ppy) {
            fout << xx << " " << yy << endl;
            ppx = xx;
            ppy = yy;
        }
    }
    fout.close();

    ct++;
}



void TongueFinder::saveImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image, 
                             const char *key) {
    char buf[256];
    sprintf(buf,"/tmp/tf_%06d_%s.ppm",ct,key);
    yarp::sig::file::write(image,(char*)buf);
}

