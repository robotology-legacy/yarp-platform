// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#ifndef FINDER_INC
#define FINDER_INC

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <string>

class Finder {
private:
  bool verbose;
public:
    Finder() : verbose(false) {}

    virtual void setVerbose(bool verbose) {
        this->verbose = verbose;
    }
    
    virtual void process(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image, 
                         yarp::sig::ImageOf<yarp::sig::PixelRgb>& out) = 0;

    int main(int argc, char *argv[]);

    bool isVerbose() { return verbose; }

    virtual std::string getName() = 0;
};

#endif


