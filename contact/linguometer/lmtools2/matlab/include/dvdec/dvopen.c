/*
 * dvopen.c
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
    int frame_size;
    char filename[256];
    int dims[] = { 1 };

    if (nrhs != 1)
        mexErrMsgTxt("Error: Must specify name of raw DV file\n");

    if (nlhs > 1)
        mexErrMsgTxt("Error: Only 1 left-hand-side argument allowed\n");

    if (!mxIsChar(prhs[0]) || mxGetM(prhs[0]) != 1)
        mexErrMsgTxt("Error: Argument 1 must be a string\n");

    dv = mxMalloc(sizeof(dv_info_t));
    mexMakeMemoryPersistent(dv);
    dv->frame_count = 0;
    dv->infile = NULL;
    dv->decoder = NULL;
    dv->buffer = NULL;

    mxGetString(prhs[0], filename, 256);
    dv->infile = fopen(filename, "r");
    if (!dv->infile) {
        mexPrintf("Error: Could not open \"%s\"\n", filename);
        goto failopen;
    }

    dv->decoder = dv_decoder_new(TRUE, FALSE, FALSE);
    if (!dv->decoder) {
        mexPrintf("Error: Could not initialize decoder\n");
        goto faildecoder;
    }

    dv->buffer = mxMalloc(120000);

    frame_size = 120000;
    if (fread(dv->buffer, 1, frame_size, dv->infile) < frame_size) {
        mexPrintf("Error: Could not read file header\n");
        goto failheader;
    }

    dv->decoder->quality = DV_QUALITY_BEST;

    if (dv_parse_header(dv->decoder, dv->buffer) < 0) {
        mexPrintf("Error: Failed to parse header\n");
        goto failheader;
    }

    fseek(dv->infile, 0, SEEK_SET);

    mxFree(dv->buffer);
    dv->buffer = mxMalloc(dv->decoder->frame_size);
    mexMakeMemoryPersistent(dv->buffer);
    plhs[0] = mxCreateNumericArray(1, dims, mxUINT32_CLASS, mxREAL);
    *(dv_info_t **)mxGetData(plhs[0]) = dv;
    return;

failheader:
    mxFree(dv->buffer);
failmem:
    dv_decoder_free(dv->decoder);
faildecoder:
    fclose(dv->infile);
failopen:
    mxFree(dv);
    mexErrMsgTxt("dvopen: aborted\n");
}
