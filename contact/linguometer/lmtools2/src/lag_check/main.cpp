/*
 *  Copyright (C) 2007 Michele Tavella <michele@liralab.it>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "mObjectDV2.h"
#include "mObjectEN2.h"
#include "mObjectPCM.h"
#include "mInclude.h"
#include "mPeakTools.h"

#define USE_DATA_0000

#ifdef USE_DATA_0000
#	define FILE_AG "data/ag_0000.wav"
#	define FILE_US "data/us_0000.wav"
#endif

#ifdef USE_DATA_0004
#	define FILE_AG "data/ag_0004.wav"
#	define FILE_US "data/us_0004.wav"
#endif

#ifdef USE_DATA_0008
#	define FILE_AG "data/ag_0008.wav"
#	define FILE_US "data/us_0008.wav"
#endif


int main (int argc, char *argv[]) {
	mObjectPCM<int16_t> *pcmAG;
	mObjectPCM<int16_t> *pcmUS;
	pcmAG = new mObjectPCM<int16_t>(MPCM_MONO, MPCM_STREAM_TINY);
	pcmUS = new mObjectPCM<int16_t>(MPCM_MONO, MPCM_STREAM_TINY);
	
	/* Read WAV files */
	mAudio2PCM(FILE_AG, 0, 0, pcmAG);
	mAudio2PCM(FILE_US, 0, 0, pcmUS);
	
	/* Free unused memory */
	pcmAG->Trim();
	pcmUS->Trim();

	int16_t *bufferAG = NULL;
	int16_t *bufferUS = NULL;
	unsigned int samplesAG;	
	unsigned int samplesUS;

	/* Get buffers */
	pcmAG->ExportBuffer(&bufferAG, samplesAG);
	pcmUS->ExportBuffer(&bufferUS, samplesUS);
	
	/* Not needed any more... */
	delete pcmAG;
	delete pcmUS;

	/* Threshold */
	threshold(bufferUS, samplesUS, US_TH);
	threshold(bufferAG, samplesAG, AG_TH);
	
	/* Remove spikes and artifacts */
	unsigned int peaksUS1;
	unsigned int peaksAG1;
	peaksUS1 = clean_spikes(bufferUS, samplesUS, US_DS_PEAK, US_DS_ARTI);
	peaksAG1 = clean_spikes(bufferAG, samplesAG, AG_DS_PEAK, AG_DS_ARTI);

#ifdef DEVELOP
	/* Now is time to look at the signals */
	mEncodePCM16("temp/agp.wav", bufferAG, NULL, samplesAG, 16000, 1);
	mEncodePCM16("temp/usp.wav", bufferUS, NULL, samplesUS, 48000, 1);
#endif

	/* After some Dumb(TM) processing, I can resample
	 * the signals, find (for each peak) its median sample
	 * and then measure the delay between the singnals!
	 * Using this method, US and AG speech should be well 
	 * aligned. Also, the estimated error should be less
	 * than 12 samples at 48kHz, that means the maximum
	 * drift in time should be around 0.00025 seconds.
	 */

	/* First of all, I set that the resapled AG signal
	 * will last as long as the US signal.
	 */
	unsigned int samplesAGR = 3 * samplesAG;
	assert(!(samplesAGR > samplesUS));
	if(samplesAGR < samplesUS)
		samplesAGR = samplesUS;

	/* Secondly, I alloc a buffer
	 */
	int16_t *bufferAGR = (int16_t *)malloc(samplesAGR * sizeof(int16_t));
	memset(bufferAGR, 0, samplesAGR * sizeof(int16_t));

#ifdef DEBUG
	/* Some spamming on the term
	 */
	printf("AG  samples: %d\n", samplesAG);
	printf("AGR samples: %d\n", samplesAGR);
	printf("US  samples: %d\n", samplesUS);
#endif

	/* Dumb(TM) resampling (sample and hold)!
	 */
	resample_linear(bufferAG, bufferAGR, samplesAG, 3, 0);
	/*
	for(unsigned int s = 0; s < samplesAG; s++) {
		if(bufferAG[s] != 0) {
			bufferAGR[3*s + 0] = bufferAG[s];
			bufferAGR[3*s + 1] = bufferAG[s];
			bufferAGR[3*s + 2] = bufferAG[s];
		}
	}
	*/

	/* Cool! US+AG speech in a single stereo file */
	mEncodePCM16("temp/syn.wav", bufferUS, bufferAGR, samplesUS, 48000, 2);
	
	unsigned int peaksUS2;
	unsigned int peaksAG2;
	peaksUS2 = median(bufferUS,  samplesUS);
	peaksAG2 = median(bufferAGR, samplesAGR);
	
	/* Let's check media values */
	mEncodePCM16("temp/med.wav", bufferUS, bufferAGR, samplesUS, 48000, 2);

	printf("AGR Peaks: %d-->%d\n", peaksAG1, peaksAG2);
	printf("US  Peaks: %d-->%d\n", peaksUS1, peaksUS2);
	assert (peaksUS2 == peaksAG2);

	unsigned int firstAGR = 0;
	unsigned int firstUS  = 0;
	firstUS  = first(bufferUS,  samplesUS);
	firstAGR = first(bufferAGR, samplesAGR);
	
	printf("AGR   First Peak: %d\n", firstAGR);
	printf("US    First Peak: %d\n", firstUS);
	printf("Delay First Peak: %d\n", firstAGR - firstUS);

	unsigned int *peaksAGR;
	unsigned int *peaksUS;
	peaksAGR = (unsigned int *)malloc(peaksAG2 * sizeof(unsigned int));
	peaksUS  = (unsigned int *)malloc(peaksUS2 * sizeof(unsigned int));
	memset(peaksAGR, 0, peaksAG2 * sizeof(unsigned int));
	memset(peaksUS,  0, peaksUS2 * sizeof(unsigned int));

	unsigned int peaksUS3;
	unsigned int peaksAG3;
	peaksAG3 = find_peak_values(bufferAGR, samplesAGR, peaksAGR);
	peaksUS3 = find_peak_values(bufferUS,  samplesUS,  peaksUS);

	printf("AGR Peaks: %d\n", peaksAG3);
	printf("US  Peaks: %d\n", peaksUS3);

	FILE *file_drift = fopen("temp/peaks_drift.txt", "w");
	if(file_drift == NULL)
		return -1;
	for(unsigned int p = 0; p < peaksUS3; p++) 
		fprintf(file_drift, "%.03d/%d/%d/%d\n", p, 
				peaksUS[p], peaksAGR[p], 
				peaksUS[p] - peaksAGR[p]);
	fclose(file_drift);

	FILE *file_dist = fopen("temp/peaks_distance.txt", "w");
	if(file_dist == NULL)
		return -1;
	for(unsigned int p = 5; p < peaksUS3 - 5; p += 2) {
		fprintf(file_dist, "%.3d-%.3d/%d/%d/%d\n", p, p - 1, 
				peaksUS[p] - peaksUS[p - 1], 
				peaksAGR[p] - peaksAGR[p - 1],
				(peaksUS[p] - peaksUS[p - 1]) -
				(peaksAGR[p] - peaksAGR[p - 1]));
	}
	fclose(file_dist);






	/* Cleaning up memory */
	free(peaksUS);
	free(peaksAGR);
	free(bufferAGR);
	free(bufferAG);
	free(bufferUS);

	return 0;
}
