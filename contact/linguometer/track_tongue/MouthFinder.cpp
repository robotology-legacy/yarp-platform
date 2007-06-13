// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "MouthFinder.h"


void MouthFinder::process(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image, 
                          yarp::sig::ImageOf<yarp::sig::PixelRgb>& out) {
    out = image;
}

