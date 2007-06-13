// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#ifndef FINDER_INC
#define FINDER_INC

class Finder {
public:
    virtual void setVerbose(bool verbose) = 0;
    
    virtual void process(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image, 
                         yarp::sig::ImageOf<yarp::sig::PixelRgb>& out) = 0;

    int main(int argc, char *argv[]);
};

#endif


