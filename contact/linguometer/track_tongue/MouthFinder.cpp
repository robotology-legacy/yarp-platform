// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "MouthFinder.h"

#include <math.h>
#include <vector>

using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace std;

double probelike(const PixelRgb& pix) {
    // devalue red, cheap bias against skin tone
    double v = pix.r+3*pix.g+3*pix.b;
    return v;
    //-fabs(pix.r-pix.g)-fabs(pix.r-pix.b)-fabs(pix.g-pix.b);
}

void MouthFinder::process(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image, 
                          yarp::sig::ImageOf<yarp::sig::PixelRgb>& out) {
    out = image;

    // we expect images with an ultrasound sensor on the right

    int ww = image.width();
    int hh = image.height();

    // get whitest pixel on right
    double best = -1e6;
    int bestIndex = 0;
    int x, y;
    for (y=0; y<hh; y++) {
        PixelRgb& pix = image(ww-1,y);
        double v = probelike(pix);
        if (v>best) {
            best = v;
            bestIndex = y;
        }
    }

    // now repeat to find range of adequately white
    int lowIndex = 10000;
    int highIndex = 0;
    double meanIndex = 0;
    double stdIndex = 0;
    double mean2Index = 0;
    int countIndex = 0;
    double threshold = best*0.8;
    for (y=0; y<hh; y++) {
        PixelRgb& pix = image(ww-1,y);
        double v = probelike(pix);
        if (v>threshold) {
            addCircle(out,PixelRgb(255,0,0),ww-1,y,10);
            if (y<lowIndex) lowIndex = y;
            if (y>highIndex) highIndex = y;
            meanIndex += y;
            mean2Index += y*y;
            countIndex++;
        }
    }
    if (countIndex>0) {
        meanIndex /= countIndex; 
        mean2Index /= countIndex;
        stdIndex = sqrt(fabs(meanIndex*meanIndex-mean2Index));
    }
    // We should now have the base of the probe, more or less.
    //addCircle(out,PixelRgb(255,0,0),ww-1,lowIndex,20);
    //addCircle(out,PixelRgb(255,0,0),ww-1,highIndex,20);

    // Trust the mean more than the bounds, probably
    addCircle(out,PixelRgb(0,255,0),ww-1,int(meanIndex),20);

    // Now look for the tip of the probe.
    int cx = int(ww-1-stdIndex*4);
    int cy =int(meanIndex);
    double rad = (stdIndex*2);

    int k = 0;
    for (k=0; k<10 && rad>1; k++) {
        addCircleOutline(out,PixelRgb(255,0,0),cx,cy,rad);
        double mx = 0;
        double my = 0;
        double ct = 0;
        for (int xx=int(cx-rad); xx<=cx+rad; xx++) {
            for (int yy=int(cy-rad); yy<=cy+rad; yy++) {
                PixelRgb& pix = image.safePixel(xx,yy);
                double v = probelike(pix);
                if (v<threshold) {
                    v = 0;
                }
                mx += v*xx;
                my += v*yy;
                ct+=v;
            }
        }
        if (ct>0.01) {
            mx /= ct;
            my /= ct;
        }
        rad *= 0.8;
        cx = int(mx);
        cy = int(my);
        //printf("at %d %d\n", cx, cy);
        cx -= int(rad*0.5); // add leftward pressure;
    }
    addCircle(out,PixelRgb(255,0,0),cx,cy,20);
    addCircle(out,PixelRgb(0,255,0),cx,cy,10);

    // now look for lips

    // basic scale defined by apparent size of probe, more or less
    int scale = (ww-1)-cx;
    int lipScale = 1+scale/5;
    vector<double> pattern;
    best = 0;
    bestIndex = 0;
    for (x=cx-lipScale*2; x>cx-scale; x--) {
        double total = 0;
        int ct = 0;
        for (int sx=-lipScale/4; sx<=lipScale/4; sx++) {
            //addCircle(out,PixelRgb(0,0,255),x+sx,cy-lipScale,5);
            //addCircle(out,PixelRgb(0,0,255),x+sx,cy+lipScale,5);
            for (y=cy-lipScale; y<=cy+lipScale; y++) {
                // very dumb code...
                PixelRgb& pix = image.safePixel(x+sx,y);
                PixelRgb& pix2 = image.safePixel(x+sx,y+lipScale/2);
                PixelRgb& pix3 = image.safePixel(x+sx+lipScale/2,y+lipScale/2);
                PixelRgb& pix4 = image.safePixel(x+sx-lipScale/2,y+lipScale/2);
                PixelRgb& pix5 = image.safePixel(x+sx+lipScale/2,y+lipScale/4);
                PixelRgb& pix6 = image.safePixel(x+sx-lipScale/2,y+lipScale/4);
                double v = pix.r+pix.g+pix.b;
                double v2 = pix2.r+pix2.g+pix2.b;
                double v3 = pix3.r+pix3.g+pix3.b;
                double v4 = pix4.r+pix4.g+pix4.b;
                double v5 = pix5.r+pix5.g+pix5.b;
                double v6 = pix6.r+pix6.g+pix6.b;
                double val = fabs(v-v2)+fabs(v-v3)+fabs(v-v4)+fabs(v-v5)+fabs(v-v6);
                total += val;
                ct++;
            }
        }
        total /= ct;
        pattern.push_back(total);
        if (total>best) {
            bestIndex = x;
            best = total;
        }
        if (total<best*0.9) {
            break;
        }
        addCircle(out,PixelRgb(0,255,0),x,int(hh-total*0.2),5);
    }


    for (y=cy-lipScale*2; y<=cy+lipScale*2; y++) {
        addCircle(out,PixelRgb(255,0,0),bestIndex,y,5);
    }
}

