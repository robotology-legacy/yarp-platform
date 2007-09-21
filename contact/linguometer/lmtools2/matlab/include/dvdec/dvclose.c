/*
 * dvclose.c
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

    if (nrhs != 1)
        mexErrMsgTxt("Error: Must specify DV handle\n");

    if (mxGetN(prhs[0]) != 1 || mxGetM(prhs[0]) != 1 || !mxIsUint32(prhs[0]))
        mexErrMsgTxt("Error: The DV handle must be a 1x1 UINT32 array\n");

    dv = *(dv_info_t **)mxGetData(prhs[0]);

    if (dv->buffer)
        mxFree(dv->buffer);
    if (dv->decoder) {
   /*     dv_decoder_free(dv->decoder); */
        if (dv->decoder->audio)
            free(dv->decoder->audio);
        if (dv->decoder->video)
            free(dv->decoder->video);
        free(dv->decoder);
    }
    if (dv->infile)
        fclose(dv->infile);
    mxFree(dv);
}
