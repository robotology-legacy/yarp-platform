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
#define	TEMP_PKS	"temp/US_peaks.wav"
#define	TEMP_ALR	"temp/sequence.alr"
#define	TEMP_ALS	"temp/sequence.als"

void help (void) {
	printf("Usage:\n");
	printf("  pdetect2 --stream file.dv --t0 time0 --t1 time1 --dd file.dd\n");
	printf("           --seg segment.wav --seq sequence.wav --pks peaks.wav --alr sequence.alr\n");
	printf("           --th 0.60 --als sequence.als\n");
	printf("  pdetect2 --help\n");
	printf("Input:\n");
	printf("  file.dv       DV file\n");
	printf("  time0         AG start-peak position (in seconds, int value)\n");
	printf("  time1         AG stop-peak position (in seconds, int value)\n");
	printf("Output:\n");
	printf("  file.dd       file containing dd's segmentation parameters\n");
	printf("  segment.wav   WAV file with the extracted segment\n");
	printf("  sequence.wav  WAV file with the extracted sequence\n");
	printf("  peaks.wav     WAV file with the extracted sequence and the peaks found\n");
	printf("  sequence.alr  ALR file with the alignement frame of reference (in samples)");
	printf("  0.60          threshold on amplitude (double)");
	printf("  sequence.als  ALS file with few alignement suggestions");
	printf("Requirments:\n");
	printf("  ffmpeg-0.4.9_p2007012 (libavcodec, libavformat)\n\n");

	cout << SAT_POS << endl;
}

void cook_dd_params (unsigned int s0, unsigned int sw0, unsigned int sw1,
	int64_t &f0, int64_t &f1) {
	unsigned int sf    = 48000;
	unsigned int vfPTS = 40000;
	int64_t p0 = mObjectDV2Tools::samples2pts(s0 + sw0, sf);
	int64_t p1 = mObjectDV2Tools::samples2pts(s0 + sw1, sf);
	
	f0 = p0/vfPTS;
	f1 = p1/vfPTS;
	int64_t f0r = p0%vfPTS;
	int64_t f1r = p1%vfPTS;
	
	if (f0r > 40000/2)
		--f0;
	if (f1r > 40000/2)
		++f1;
}

bool detect_segment(mObjectPCM<int16_t> *pcm,
		unsigned int &sequence_s0, unsigned int &sequence_s1) {
	int s0 = -1;
	int s1 = -1;
	
	for(unsigned int s = 0; s < pcm->getSamples(); s++) {
		if(pcm->getL(s) < US_TH*SAT_NEG_US && s0 == -1)
			s0 = s;
		else if(pcm->getL(s) > US_TH*SAT_POS_US && s1 == -1) 
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
	unsigned int rateUS = 48000;
#ifdef DEVELOP
	printf("--> Running in DEVELOP mode. Unpredictable behavior if no temp/ folder exists\n");
#endif
	int segmentRough_t0 = 0;
	int segmentRough_t1 = 0;
	float segment_th = 0.00;
	char *filename_US = NULL;
	char *filename_DD = NULL;
	char *filename_SEG = NULL;
	char *filename_SEQ = NULL;
	char *filename_ALR = NULL;
	char *filename_PKS = NULL;
	char *filename_ALS = NULL;
#ifdef DEVELOP	
	segmentRough_t0 = TEMP_T0;
	segmentRough_t1 = TEMP_T1;

	filename_US = TEMP_USDV;
	filename_DD = TEMP_DD;
	filename_SEG = TEMP_SEG;
	filename_SEQ = TEMP_SEQ;
	filename_ALR = TEMP_ALR;
	filename_PKS = TEMP_PKS;
	filename_ALS = TEMP_ALS;
#else
	/* Parsing starts here */
	switch (argc) {
		case 2:
			if (strcmp(argv [1], "--help") == 0) {
				welcome();
				help();
				return 0;
			}
			break;
		case 21:
			if (strcmp(argv [1], "--stream") == 0 &&
					strcmp(argv [3], "--t0") == 0 &&
					strcmp(argv [5], "--t1") == 0 &&
					strcmp(argv [7], "--dd") == 0 &&
					strcmp(argv [9], "--seg") == 0 &&
					strcmp(argv [11], "--seq") == 0 &&
					strcmp(argv [13], "--pks") == 0 &&
					strcmp(argv [15], "--alr") == 0 &&
					strcmp(argv [17], "--th") == 0 &&
					strcmp(argv [19], "--als") == 0) {
				filename_US = argv [2];
				assert(sscanf(argv [4], "%d", &segmentRough_t0) == 1);
				assert(sscanf(argv [6], "%d", &segmentRough_t1) == 1);
				filename_DD = argv [8];
				filename_SEG = argv [10];
				filename_SEQ = argv [12];
				filename_PKS = argv [14];
				filename_ALR = argv [16];
				assert(sscanf(argv [18], "%f", &segment_th) == 1);
				filename_ALS = argv [20];
			}
			else
				return -1;
			break;
		default:
			welcome();
			help();
			return 0;
			break;
	}
	/* Parsing stops here */
#endif

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

	/* Threshold on amplitudes:
	 * Easily detect the spikes. The lower is the threshold
	 * value, the higher will be the number of the detected peaks.
	 * Those peaks could be Ham (if the lenght ~ 60samples) 
	 * or Spam (if the lenght </> 60samples) or could even 
	 * be ghost peaks.
	 * Since ghost-peak recovery could be expensive in terms
	 * of resources, it is quite useful to adopt this
	 * strategy:
	 * - Use a "lower" threshold, e.g. 60% of the max value.
	 *   The ideal value is 80%, but we could skip peaks 
	 *   that are not that bad...
	 * - Using lower threshold values means that we'll collect
	 *   ham and lots of spam, the spam filtering is done 
	 *   on the peaks durations (clean_spikes)
	 */
#ifdef USE_SAT_INVERT
	negative(bPeaksSEQ, samplesSEQ);
#endif
	threshold(bPeaksSEQ, samplesSEQ, (double)segment_th);

	/* Threshold on duration 
	 * A peak, ideally, is 64 samples. 
	 */
	unsigned int my_peaks = 0;
	clean_spurious_spikes(bPeaksSEQ, samplesSEQ);
	my_peaks = clean_spikes(bPeaksSEQ, samplesSEQ, US_DS_PEAK, US_DS_ARTI);
	clean_spurious_spikes(bPeaksSEQ, samplesSEQ);

#ifdef DEVELOP 
	/* Save peak detection results, with broken peaks */
	mEncodePCM16("temp/US_threshold.wav", bufferSEQ, bPeaksSEQ, samplesSEQ, rateUS, 2);
#endif	
	/* Peak detection 
	 * Peak frame of reference referred to the beginning of the sequence
	 */
	WD_peaks sPeaksSEQ;
	memset(&sPeaksSEQ, 0, sizeof(WD_peaks));
	find_peaks(bPeaksSEQ, samplesSEQ, sPeaksSEQ, 0);
	
	/* Sequence peak-consistency check */
	bool start_ok = check_sequence_start(sPeaksSEQ);
	bool stop_ok = check_sequence_stop(sPeaksSEQ);
	printf("Sequence peak-consistency:\n");
	printf("     Start: %s\n", start_ok ? "passed" : "failed");
	printf("     Stop:  %s\n", stop_ok ? "passed" : "failed");
	
	/* Open the ALS file */
	FILE *FILE_ALS = fopen(filename_ALS, "w");
	if(FILE_ALS == NULL)
		return -1;
	
	bool ghost_nuked = false;
	unsigned int ghost_total = 0;
	unsigned int delta_p = 2;
	do{
		for(unsigned int p = 3; p < sPeaksSEQ.tot - 3; p += delta_p) {
			if(sPeaksSEQ.type[p] == SAT_POS && sPeaksSEQ.type[p + 1] == SAT_NEG) {
				
				/* Format: word, peak in WD data, peak in SEQ data*/
				//printf("[NN] --> %d/%d/%d\n", (p - 3)/2, 0, p);
				fprintf(FILE_ALS, "%d/%d/%d\n", (p - 3)/2, 0, p - ghost_total);
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

				if(sPeaksSEQ.type[prev_peak] == SAT_NEG && sPeaksSEQ.type[this_peak] == SAT_POS && sPeaksSEQ.type[next_peak] == SAT_POS) {
					printf("    Ghost peak [-SAT]: between %d-%d peak, type should be %d\n",
							this_peak, next_peak,
							int16_t(SAT_NEG));

					fprintf(FILE_ALS, "%d/%d/%d\n", (this_peak - 3)/2, 0, this_peak - ghost_total);
					ghost_total++;
					
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
#ifdef DEVELOP 
					mEncodePCM16("temp/ghost_found.wav", bufferGST, NULL, recovery_ds, rateUS, 1);
#endif
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
#ifdef DEVELOP
					mEncodePCM16("temp/ghost_recovered.wav", bufferGST,
							bPeaksSEQ + recovery_s0, recovery_ds, rateUS, 2);
#endif

					printf("    Peak sequence recovered. Running peak detection again\n");
					memset(&sPeaksSEQ, 0, sizeof(WD_peaks));
					find_peaks(bPeaksSEQ, samplesSEQ, sPeaksSEQ, 0);
					printf("    Peak detection completed!\n");
					delta_p = 2;
					free(bufferGST);
				}
				else if(sPeaksSEQ.type[prev_peak] == SAT_NEG && sPeaksSEQ.type[this_peak] == SAT_NEG && sPeaksSEQ.type[next_peak] == SAT_POS) {
					printf("    Ghost peak [+SAT]: between %d-%d peak, type should be %d\n",
							prev_peak, this_peak,
							int16_t(SAT_POS));

					fprintf(FILE_ALS, "%d/%d/%d\n", (this_peak - 4)/2, 1, this_peak - ghost_total);
					ghost_total++;
				
					/* Ghost peak recovery */
					/* The peak should be circa bewteen recovery_s0 and *_s1 */
					unsigned int recovery_s0 = sPeaksSEQ.stop[prev_peak] + (unsigned int)(0.25 * rateUS);
					unsigned int recovery_s1 = recovery_s0 + (unsigned int)(0.70 * rateUS);
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
#ifdef DEVELOP 
					mEncodePCM16("temp/ghost_found.wav", bufferGST, NULL, recovery_ds, rateUS, 1);
#endif
					/* Fetch for the peak. I use few threshold values and 
					 * then I look for the values higher thant the threshold.
					 */
					double thresholds[5] = {0.05, 0.10, 0.20, 0.40, 0.60};
					unsigned int ghost_samples[5] = {0, 0, 0, 0, 0};
					for(unsigned int th = 0; th < 5; th++) {
						for(unsigned int sg = recovery_s0; sg < recovery_s1; sg++) {
							if(bufferSEQ_copy[sg] >= thresholds[th]*SAT_POS) 
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
						if(bufferSEQ_copy[sg] >= thresholds[ghost_th]*SAT_POS)
							bPeaksSEQ[sg] = SAT_POS;
					}
#ifdef DEVELOP
					mEncodePCM16("temp/ghost_recovered.wav", bufferGST,
							bPeaksSEQ + recovery_s0, recovery_ds, rateUS, 2);
#endif

					printf("    Peak sequence recovered. Running peak detection again\n");
					memset(&sPeaksSEQ, 0, sizeof(WD_peaks));
					find_peaks(bPeaksSEQ, samplesSEQ, sPeaksSEQ, 0);
					printf("    Peak detection completed!\n");
				
					delta_p = 2;
					free(bufferGST);
				}
				else {
					printf("Sorry, a Ghost-Peak exists but cannot be recovered.\n");
					delta_p = 1;
				}
			}
		}
	} while(ghost_nuked == false);
	fclose(FILE_ALS);
	
	
	mEncodePCM16(filename_PKS, bufferSEQ, bPeaksSEQ, samplesSEQ, rateUS, 2);
	
	/* Spam */
	printf("Found peaks:\n");
	print_peaks(sPeaksSEQ);
	
	/* Open dd log file and write, for each word, the pevious and the 
	 * next video frame
	 */
	FILE *FILE_DD = fopen(filename_DD, "w");
	if(FILE_DD == NULL)
		return -1;
	
	unsigned int word = 0;
	int64_t dd_f0 = 0;
	int64_t dd_f1 = 0;
	for(unsigned int p = 3; p < sPeaksSEQ.tot - 3; p += 2) {
		if(sPeaksSEQ.type[p] == SAT_POS && sPeaksSEQ.type[p + 1] == SAT_NEG) {
			cook_dd_params(delta_s + sequence_s0,
					sPeaksSEQ.start[p] - 6000, sPeaksSEQ.stop[p + 1] + 6000,
					dd_f0, dd_f1);
			fprintf(FILE_DD, "%d/%d/%d\n", word, (int)dd_f0, (int)(dd_f1 - dd_f0 + 1));
			word++;
		}
	}
	fclose(FILE_DD);
	
	printf("Exiting:\n");
	printf("    Total number of peaks:    %d\n", sPeaksSEQ.tot);
	printf("    Expected number of words: %d\n", (sPeaksSEQ.tot - 6)/2);

	/* Cleaning... */
	free(bufferSEG_SN);
	free(bufferSEG_WD);
	free(bufferSEQ);
	free(bufferSEQ_copy);
	free(bPeaksSEQ);

	return 0;
}


