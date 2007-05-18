/* Coded by M. Tavella
 * email michele@liralab.it
 * web   http://mt.homelinux.org 
 */
#include "mObjectDV2.h"
#include "mObjectEN2.h"
#include "mObjectPCM.h"
#include "mObjectIMG.h"
#include "mInclude.h"
#include "mPeakTools.h"

#define TEMP_USDV	"data/stream.dv"
#define TEMP_T0		813
#define TEMP_T1		1001
#define TEMP_DD		"temp/sequence.dd"
#define	TEMP_SEG	"temp/US_segment.wav"
#define	TEMP_SEQ	"temp/US_sequence.wav"
#define	TEMP_ALR	"temp/sequence.alr"

void help (void) {
	printf("Usage:\n");
	printf("  pdetect --stream file.dv --t0 time0 --t1 time1 --log file.log\n");
	printf("    --seg segment.wav --seq sequence.wav\n");
	printf("  pdetect --help\n");
	printf("Where:\n");
	printf("  file.dv   DV file\n");
	printf("  time0     AG start-peak position (in seconds, int value)\n");
	printf("  time1     AG stop-peak position (in seconds, int value)\n");
	printf("  file.log  file containing dd's segmentation parameters\n");
	printf("Requirments:\n");
	printf("  ffmpeg-0.4.9_p2007012 (libavcodec, libavformat)\n\n");
}


bool detect_segment(mObjectPCM<int16_t> *pcm,
		unsigned int &sequence_s0, unsigned int &sequence_s1) {
	int s0 = -1;
	int s1 = -1;
	
	for(unsigned int s = 0; s < pcm->getSamples(); s++) {
		if(pcm->getL(s) < US_TH*SAT_NEG && s0 == -1)
			s0 = s;
		else if(pcm->getL(s) > US_TH*SAT_POS && s1 == -1) 
			s1 = s;
		if(s0 > -1 && s1 > -1)
			break;
	}

	if (s0 > 0 && s1 > s0) {
		sequence_s0 = (unsigned int)s0;
		sequence_s1 = (unsigned int)s1;
	}
	else
		return false;

	return true;
}

int main (int argc, char *argv[]) {
	char *filename_US = TEMP_USDV;
	int segmentRough_t0 = TEMP_T0;
	int segmentRough_t1 = TEMP_T1;
	
	char *filename_DD = TEMP_DD;
	char *filename_SEG = TEMP_SEG;
	char *filename_SEQ = TEMP_SEQ;
	char *filename_ALR = TEMP_ALR;

	unsigned int rateUS = 48000;


	/* Parsing starts here */
	/*
	 *
	 *
	 */
	/* Parsing stops here */

	/* We need to convert the rough time values (in seconds) into
	 * pts values (in u-seconds)
	 */
	int64_t segmentRough_pts0 = mObjectDV2Tools::secs2pts(segmentRough_t0);
	int64_t segmentRough_pts1 = mObjectDV2Tools::secs2pts(segmentRough_t1);
	unsigned int delta_s = mObjectDV2Tools::pts2samples(segmentRough_pts0, rateUS);

	/* mObjectPCM is required to extract the PCM stream from the
	 * DV file filename_US
	 */
	mObjectPCM<int16_t> *pcmUS = NULL;
	pcmUS = new mObjectPCM<int16_t>(MPCM_STEREO, MPCM_STREAM_SMALL);
	
	/* PCM extraction and trimming
	 */
	mDV2PCM(filename_US, segmentRough_pts0, segmentRough_pts1, pcmUS);
	pcmUS->Trim();

	/* AG-Segment detection: returns fine values in samples
	 */
	unsigned int sequence_s0 = 0;
	unsigned int sequence_s1 = 0;
	bool sequenceResult = detect_segment(pcmUS, sequence_s0, sequence_s1);
	assert(sequenceResult);
	
	/* Ham */
	printf("\nAG-Sequence detected:\n");
	printf("    s0:    %d samples\n", sequence_s0);
	printf("    s1:    %d samples\n", sequence_s1);

	/* ALR - Alignment frame of reference */
	unsigned int segmentALR = segmentRough_t0 * rateUS + sequence_s0;
	printf("Segment ALR values:\n");
	printf("    rough t0/s0:    %d seconds/%d samples\n", 
			segmentRough_t0, segmentRough_t0 * rateUS);
	printf("    rough t1/s0:    %d seconds/%d samples\n", 
			segmentRough_t1, segmentRough_t1 * rateUS);
	printf("    ALR value:      %d samples\n", segmentALR);

	/* Lets keep the ALR value, it will be used
	 * during blind-peak alignment (lm_postcc)
	 */
	FILE *FILE_ALR = fopen(filename_ALR, "w");
	if (FILE_ALR == NULL)
		return -1;
	fprintf(FILE_ALR, "%d\n", segmentALR);
	fclose(FILE_ALR);

	/* Buffer WD keeps the word signal (right channel) 
	 * while bufferSN keeps the sync signal (left channel),
	 * both from the US PCM stream.
	 * It's buffer time!
	 */
	unsigned int samplesSEG;
	int16_t *bufferSEG_SN = NULL;
	int16_t *bufferSEG_WD = NULL;
	pcmUS->ExportBuffers(&bufferSEG_SN, &bufferSEG_WD, samplesSEG);

	/* Not needed any more... mObjectPCM is slow,
	 * and alterates my signal perception.
	 */
	delete pcmUS;

	
	/* Before going any further, let's save the detected
	 * segment
	 */
	printf("Saving SEG: %s:\n", filename_SEG);
	mEncodePCM16(filename_SEG, bufferSEG_SN, bufferSEG_WD, samplesSEG, rateUS, 2);
	
	/* A properly trimmed segment is a sequence 
	 */
	unsigned int samplesSEQ = sequence_s1 - sequence_s0 + 1;
	int16_t *bufferSEQ = NULL;
	bufferSEQ = (int16_t *)malloc(samplesSEQ * sizeof(int16_t));
	memcpy(bufferSEQ, bufferSEG_WD + sequence_s0, samplesSEQ * sizeof(int16_t));	

	/* Let's save the detected sequence */
	printf("Saving SEQ: %s \n", filename_SEQ);
	mEncodePCM16(filename_SEQ, bufferSEQ, NULL, samplesSEQ, rateUS, 1);

	/* Let's keep in memory a copy of the sequence */
	int16_t *bufferSEQ_copy = NULL;
	bufferSEQ_copy = (int16_t *)malloc(samplesSEQ * sizeof(int16_t));
	memcpy(bufferSEQ_copy, bufferSEQ, samplesSEQ * sizeof(int16_t));	
	
	/* Let's create a copy of SEQ suitable for peak detection */
	int16_t *bPeaksSEQ = NULL;
	bPeaksSEQ = (int16_t *)malloc(samplesSEQ * sizeof(int16_t));
	memcpy(bPeaksSEQ, bufferSEQ, samplesSEQ * sizeof(int16_t));	


	/* Threshold */
	//threshold(bPeaksSEQ, samplesSEQ, US_TH);
	threshold(bPeaksSEQ, samplesSEQ, 0.80);
	mEncodePCM16("temp/sequence_th.wav", bufferSEQ, bPeaksSEQ, samplesSEQ, rateUS, 2);

	/* Peak detection */
	WD_peaks sPeaksSEQ;
	memset(&sPeaksSEQ, 0, sizeof(WD_peaks));
	/* Peak frame of reference referred to the beginning of US stream */
	//find_peaks(bPeaksSEQ, samplesSEQ, sPeaksSEQ, delta_s);
	/* Peak frame of reference referred to the beginning of the sequence */
	find_peaks(bPeaksSEQ, samplesSEQ, sPeaksSEQ, 0);
	
	/* Sequence peak-consistency check */
	bool start_ok = check_sequence_start(sPeaksSEQ);
	bool stop_ok = check_sequence_stop(sPeaksSEQ);
	printf("Sequency peak-consistency:\n");
	printf("     Start: %s\n", start_ok ? "passed" : "failed");
	printf("     Stop:  %s\n", stop_ok ? "passed" : "failed");
	
	/* Spam */
	printf("Found peaks:\n");
	//print_peaks(sPeaksSEQ);
	bool ghost_nuked = false;	
	unsigned int delta_p = 2;
	do{
		for(unsigned int p = 3; p < sPeaksSEQ.tot - 3; p += delta_p) {
			if(sPeaksSEQ.type[p] == SAT_POS && sPeaksSEQ.type[p + 1] == SAT_NEG) {
				/*
				   printf("Word detected:\n");
				   printf("    Peaks:   %d and %d\n", p, p + 1);
				   printf("    Samples: %d and %d\n", sPeaksSEQ.stop[p], sPeaksSEQ.start[p + 1]);
				 */
				delta_p = 2;

				/* If we did reach the last +SAT peak we are set. No
				 * Ghost-Peaks are left
				 */
				if(p + 1 == sPeaksSEQ.tot - 4) {
					printf("Peak detection and peak correction is finished!\n");
					ghost_nuked = true;
				}
			}
			else {
				unsigned int this_peak = p;
				unsigned int prev_peak = p - 1;
				unsigned int next_peak = p + 1;
				printf("Word not detected:\n");
				printf("    Prev peak: %d peak, type %d\n", prev_peak, sPeaksSEQ.type[prev_peak]);
				printf("    This peak: %d peak, type %d\n", this_peak, sPeaksSEQ.type[this_peak]);
				printf("    Next peak: %d peak, type %d\n", next_peak, sPeaksSEQ.type[next_peak]);

				if(sPeaksSEQ.type[prev_peak] == SAT_NEG && sPeaksSEQ.type[next_peak] == SAT_POS) {
					printf("    Ghost peak: between %d-%d peak, type should be %d\n",
							this_peak, next_peak,
							int16_t(SAT_NEG));

					/* Ghost peak recovery */
					/* The peak should be circa bewteen recovery_s0 and *_s1 */
					unsigned int recovery_s1 = sPeaksSEQ.start[next_peak] - (unsigned int)(0.35 * rateUS);
					unsigned int recovery_s0 = recovery_s1 - (unsigned int)(0.70 * rateUS) + 1; 
					unsigned int recovery_ds = recovery_s1 - recovery_s0 + 1;
					printf("       Looking: between %d-%d peak (delta = %d)\n",
							recovery_s0, recovery_s1, recovery_ds);

					/* Allocate a Ghost-Buffer and copy over there the 
					 * ghost-peak interval.
					 * Also, dump the buffer on the disk.
					 */
					int16_t *bufferGST = NULL;
					bufferGST = (int16_t *)malloc(recovery_ds * sizeof(int16_t));
					memcpy(bufferGST, bufferSEQ_copy + recovery_s0, recovery_ds * sizeof(int16_t));	
					mEncodePCM16("temp/ghost_found.wav", bufferGST, NULL, recovery_ds, rateUS, 1);

					/* Fetch for the peak. I use few threshold values and 
					 * then I look for the values higher thant the threshold.
					 */
					double thresholds[5] = {0.05, 0.10, 0.20, 0.40, 0.60};
					unsigned int ghost_samples[5] = {0, 0, 0, 0, 0};
					for(unsigned int th = 0; th < 5; th++) {
						for(unsigned int sg = recovery_s0; sg < recovery_s1; sg++) {
							if(bufferSEQ_copy[sg] <= thresholds[th]*SAT_NEG) 
								ghost_samples[th]++;
						}
						printf("    Fetching with th=%f, found samples=%d\n", 
								thresholds[th], ghost_samples[th]);
					}

					unsigned int ghost_th = 0;
					unsigned int ghost_diff = 2*rateUS;
					for(unsigned int th = 0; th < 5; th++) {
						if(70 - ghost_samples[th] < ghost_diff) {
							ghost_diff = 70 - ghost_samples[th];
							ghost_th = th;
						}
					}
					printf("    Winner th=%f, found samples=%d\n", 
							thresholds[ghost_th], ghost_samples[ghost_th]);
					if(ghost_samples[ghost_th] > 50 && ghost_samples[ghost_th] < 80) 
						printf("    Ghost peak validated. Recovering\n");

					for(unsigned int sg = recovery_s0; sg < recovery_s1; sg++) {
						if(bufferSEQ_copy[sg] <= thresholds[ghost_th]*SAT_NEG)
							bPeaksSEQ[sg] = SAT_NEG;
					}
					mEncodePCM16("temp/ghost_recovered.wav", bufferGST,
							bPeaksSEQ + recovery_s0, recovery_ds, rateUS, 2);

					printf("    Peak sequence recovered. Running peak detection again\n");
					memset(&sPeaksSEQ, 0, sizeof(WD_peaks));
					find_peaks(bPeaksSEQ, samplesSEQ, sPeaksSEQ, 0);
					printf("    Peak detection completed!\n");
					delta_p = 2;
				}
				else {
					printf("Sorry, a Ghost-Peak exists but I cannot recover it\n");
					delta_p = 1;
				}
			}
		}
	} while(ghost_nuked == false);
	print_peaks(sPeaksSEQ);

	/* Cleaning... */
	free(bufferSEG_SN);
	free(bufferSEG_WD);
	free(bufferSEQ);
	free(bufferSEQ_copy);
	free(bPeaksSEQ);

	return 0;
}


