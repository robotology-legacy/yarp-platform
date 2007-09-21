/*
 * dvread.c
 * David Moore <dcm@csail.mit.edu>
 *
 * Copyright (C) 2004  Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <stdio.h>
#include <libdv/dv.h>
#include "mex.h"
#include "dvmex.h"

void
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    dv_info_t * dv;
    int dims[3];
    int ts[4];
    unsigned char * pixels[1], *inbuf, *outbuf;
    int pitches[1];
    int i, j, k, row, plsize;

    if (nrhs != 1)
        mexErrMsgTxt("Error: Must specify DV handle\n");

    if (mxGetN(prhs[0]) != 1 || mxGetM(prhs[0]) != 1 || !mxIsUint32(prhs[0]))
        mexErrMsgTxt("Error: The DV handle must be a 1x1 UINT32 array\n");

    dv = *(dv_info_t **)mxGetData(prhs[0]);

    if (nlhs > 2)
        mexErrMsgTxt("Error: No more than 2 left-hand-side arguments allowed\n");

    if (fread(dv->buffer, 1, dv->decoder->frame_size, dv->infile)
            < dv->decoder->frame_size) {
        if (nlhs < 1)
            return;
        plhs[0] = mxCreateNumericArray(0, dims, mxUINT8_CLASS, mxREAL);
        if (nlhs < 2)
            return;
        plhs[1] = mxCreateDoubleMatrix(0, 0, mxREAL);
        return;
    }
    
    if (dv_parse_header (dv->decoder, dv->buffer) < 0) {
        mexErrMsgTxt("Error: Failed to parse frame header\n");
    }

    dv_parse_packs(dv->decoder, dv->buffer);
    
    dv_get_timestamp_int(dv->decoder, ts);
    /*mexPrintf("Frame %d at %02d:%02d:%02d.%02d\n",
            dv->frame_count, ts[0], ts[1], ts[2], ts[3]);*/

    dv->frame_count++;
    if (nlhs < 1)
        return;

    pixels[0] = mxMalloc(dv->decoder->width * dv->decoder->height * 3);
    pitches[0] = dv->decoder->width * 3;

    dv_decode_full_frame(dv->decoder, dv->buffer, e_dv_color_rgb,
            pixels, pitches);

    dims[0] = dv->decoder->height;
    dims[1] = dv->decoder->width;
    dims[2] = 3;
    plhs[0] = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);

    inbuf = pixels[0];
    outbuf = (unsigned char *) mxGetData(plhs[0]);
    plsize = dv->decoder->width * dv->decoder->height;
    for (i = 0; i < dv->decoder->height; i++) {
        for (j = 0; j < dv->decoder->width; j++) {
            int row = j * dv->decoder->height;
            for (k = 0; k < 3; k++) {
                outbuf[k*plsize + row + i] = *inbuf;
                inbuf++;
            }
        }
    }
    mxFree(pixels[0]);

    if (nlhs < 2)
        return;

    dims[0] = 4;
    plhs[1] = mxCreateDoubleMatrix(4, 1, mxREAL);
    *(mxGetPr(plhs[1])+0) = ts[0];
    *(mxGetPr(plhs[1])+1) = ts[1];
    *(mxGetPr(plhs[1])+2) = ts[2];
    *(mxGetPr(plhs[1])+3) = ts[3];

    return;
}
