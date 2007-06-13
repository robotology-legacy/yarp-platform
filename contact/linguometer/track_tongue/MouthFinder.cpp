// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "MouthFinder.h"
#include "YARPViterbi.h"

#include <math.h>
#include <vector>

using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace std;

double point(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image, 
             int x, int y, int dir) {
    PixelRgb& pix1 = image.safePixel(x,y);
    //return -pix1.r;
    PixelRgb& pix2 = image.safePixel(x+5,y);
    PixelRgb& pix3 = image.safePixel(x-5,y);
    // having darkness to side is good; being red is good
    return (pix2.r-pix1.r) + (pix3.r-pix1.r) - 3*(pix1.r-pix1.g);
}


double comp(PixelRgb pix1, PixelRgb pix2) {
    double t1 = (pix1.r + pix1.g + pix1.b)/3.0;
    double t2 = (pix2.r + pix2.g + pix2.b)/3.0;
    if (t1<50) t1 = 50;
    if (t2<50) t2 = 50;
    double r1 = pix1.r/t1;
    double g1 = pix1.g/t1;
    double r2 = pix2.r/t2;
    double g2 = pix2.g/t2;
    double d1 = 255*(r1-r2);
    double d2 = 255*(g1-g2);
    double v = d1*d1+d2*d2;
    return v;
}

double match(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image, 
             int x1, int y1,
             int x2, int y2) {
    PixelRgb& pix1 = image.safePixel(x1,y1);
    PixelRgb& pix2 = image.safePixel(x2,y2);
    double change = fabs(pix1.r-pix2.r);

    // we don't like change on our contour

    return change;
}

double probelike(const PixelRgb& pix) {
    // devalue red, cheap bias against skin tone
    double v = pix.r+3*pix.g+3*pix.b;
    return v;
    //-fabs(pix.r-pix.g)-fabs(pix.r-pix.b)-fabs(pix.g-pix.b);
}


class Stater {
private:
    int scale;
    int unit;
public:
    Stater() {
        scale = 100;
        unit = 1;
    }

    void setScale(int scale, int unit = 1) {
        this->scale = scale;
        this->unit = unit;
    }
    
    int getStates() {
        // integer
        return (scale/unit)*(scale/unit);
    }

    int toState(int width, int offset) {
        return (scale/unit)*(width/unit)+offset/unit;
    }

    int toOffset(int state) {
        state %= (scale/unit);
        state *= unit;
        return state;
    }

    int toWidth(int state) {
        state /= (scale/unit);
        state *= unit;
        return state;
    }
};



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
        addCircleOutline(out,PixelRgb(255,0,0),cx,cy,(int)rad);
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
    int xref = cx;
    int yref = cx+lipScale;
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
        //addCircle(out,PixelRgb(0,255,0),x,int(hh-total*0.2),5);
    }

    int xLip = bestIndex;
    int yLip = cy;

    for (y=int(cy-lipScale*2.5); y<=cy+lipScale*2.5; y++) {
        addCircle(out,PixelRgb(255,0,0),xLip,y,2);
    }

    // Ok, in principle we now know roughly where the mouth is
    // Now let's try to learn something about its shape


    ImageOf<PixelFloat> curr,diff;
    static ImageOf<PixelFloat> prev;
    curr.copy(image);
    if (prev.width()==0) {
        prev.copy(curr);
    }
    
    diff.resize(curr);
    IMGFOR(diff,x,y) {
        if (fabs(x-xLip)<lipScale) {
            double v = fabs(curr(x,y)-prev(x,y));
            v *= 5;
            if (v>255) v = 255;
            diff(x,y) = v;
            if (v>128) {
                int w = (int)v;
                out(x,y) = PixelRgb(0,w,0);
            }
        }
    }
    //out.copy(diff);


    /*

    int del = 0;
    int x0 = xLip+del;
    int y0 = yLip-int(lipScale*3);
    int x1 = xLip+del;
    int y1 = yLip+int(lipScale*3);

    YARPViterbi path;
    Stater state;
    state.setScale(lipScale,1+lipScale/20);
    int nstate = state.getStates()+5; // 5 just to avoid out-by-one allocation
    int nseq = y1-y0+5;               // ditto
    path.SetSize(nstate,nseq);
    int n = state.getStates();
    path.Reset();
    for (y=y0; y<=y1; y++) {
        path.BeginTransitions();
        for (int n0=0; n0<n; n0++) {
            int o0 = state.toOffset(n0);
            int w0 = state.toWidth(n0);
            double local = 0;
            int xlo = x0+o0-w0/2;
            int xhi = x0+o0+w0/2;
            int xmid = x0+o0;
            PixelRgb& plo = image.safePixel(xlo,y);
            PixelRgb& phi = image.safePixel(xhi,y);
            PixelRgb& plop = image.safePixel(xlo-4,y);
            PixelRgb& phip = image.safePixel(xhi+4,y);
            PixelRgb& pref = image.safePixel(xref,yref);
            //local -= plo.r + phi.r;
            //local += pmid.r;
            //local += w0;
            double intern = 0;
            double ref = comp(phi,plo)+0.001;
            int ct = 0;
            for (x=xlo+1; x<xhi; x++) {
                PixelRgb& pix = image.safePixel(x,y);
                //double d1 = comp(plo,pix);
                //double d2 = comp(phi,pix);
                double v = fabs(pix.r-plo.r) + fabs(pix.r-phi.r);
                if (v>intern) intern = v;
                ct++;
            }
            //if (ct>0) intern /= ct;
            //intern /= (xhi-xlo+1);
            //local += comp(plo,phi)*w0*0.1;
            //local -= comp(phi,pmid)*w0;
            //local -= comp(plo,pmid)*w0;
            local -= intern;
            local -= plo.r/2;
            local -= phi.r/2;
            local += w0;
            local += fabs(plo.r-plop.r)/2;
            local += fabs(phi.r-phip.r)/2;

            bool ok = true;

            if (w0>0) {
                for (int n1=0; n1<n; n1++) {
                    int o1 = state.toOffset(n1);
                    int w1 = state.toWidth(n1);
                    double delta = fabs(o0-o1)+fabs(w0-w1)/4;
                    if (delta<=8.5) {
                        path.AddTransition(n0,n1,local+delta*30);
                    }
                }
            }
        }
        path.EndTransitions();
    }

    path.CalculatePath();
    
    for (y=y0; y<=y1; y+=1) {
        int n0 = path(y-y0);
        int o0 = state.toOffset(n0);
        int w0 = state.toWidth(n0);
        //printf("%d %d\n", o0, w0);
        addCircle(out,PixelRgb(0,255,0),x0+o0-w0/2,y,2);
        addCircle(out,PixelRgb(0,0,255),x0+o0+w0/2,y,2);
    }

*/    

    /*
    Stater state;
    state.setScale(100,10);
    int n = state.toState(40,32);
    printf("%d %d\n", state.toWidth(n), state.toOffset(n));
    exit(1);
    */

    /*

    ImageOf<int> mask;
    mask.resize(image);
    mask.zero();
    int top = 0;
    for (int del=-20; del<=20; del += 1) {
        int x0 = xLip+del;
        int y0 = yLip-int(lipScale*2.5);
        int x1 = xLip+del;
        int y1 = yLip+int(lipScale*2.5);
        // find a good path from (x0,y0) to (x1,y1) along a vague contour

        int s = 1;
        YARPViterbi path;
        int nstate = lipScale*2+5; // 5 is just to avoid out-by-one allocation
        int nseq = y1-y0+5;        // ditto
        path.SetSize(nstate,nseq);
        int costChange = 100;
        path.Reset();
        path.BeginTransitions();
        path.AddTransition(lipScale,lipScale,0);
        path.EndTransitions();
        for (y=y0+1; y<y1; y++) {
            path.BeginTransitions();
            for (x=-lipScale; x<=lipScale; x++) {
                double local = point(image,x+x0,y,s);
                path.AddTransition(x+lipScale,x+lipScale,
                                   local+match(image,x+x0,y,x+x0,y+1));
                if (x>-lipScale) {
                    path.AddTransition(x+lipScale,x+lipScale-1,
                                       local+costChange+
                                       match(image,x+x0,y,x+x0-1,y+1));
                }
                if (x<lipScale) {
                    path.AddTransition(x+lipScale,x+lipScale+1,
                                       local+costChange+
                                       match(image,x+x0,y,x+x0+1,y+1));
                }
            }
            path.EndTransitions();
        }
        path.BeginTransitions();
        path.AddTransition(lipScale,lipScale,0);
        path.EndTransitions();
        
        path.CalculatePath();
        //path.ShowPath();
        
        for (y=y0; y<=y1; y++) {
            int val = mask.safePixel(path(y-y0)+x0-lipScale,y)++;
            if (val>top) {
                top = val;
            }
            //addCircle(out,PixelRgb(0,(s>0)*255,(s<0)*255),path(y-y0)+x0-lipScale,y,1);
        }
    }
    if (top>0) {
        IMGFOR(mask,x,y) {
            int pix = mask(x,y);
            if (pix>5) {
                int v = int(255*(float)pix/top);
                if (v>255) v = 255;
                addCircle(out,PixelRgb(0,v,0),x,y,2);
            }
        }
    }

    */

}

