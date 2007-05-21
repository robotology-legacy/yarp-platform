/* Coded by M. Tavella
 * email michele@liralab.it
 * web   http://mt.homelinux.org 
 */
#include "mObjectDV2.h"
#include "mObjectEN2.h"
#include "mObjectPCM.h"
#include "mInclude.h"
#include "mPeakTools.h"

//#define DEVELOP
void help (void) {
	printf("Usage:\n");
	printf(" word_align --sequence sequence.wav --wd word_in.wav --num 10\n");
	printf("            --out word_out.wav --log file.aln --pwd 0 --pag 89\n");
	printf("Where:\n");
	printf("  sequence.wav   AG sequence (16kHz, Mono)\n");
	printf("  word_in.wav    US word  (48kHz, Stereo)\n");
	printf("  10             US word number (in sequence)\n");
	printf("  word_out.wav   AG segmented word (16kHz, Mono)\n");
	printf("  file.aln       Sync informations\n");
	printf("  0              WD peak used for alignement (0 or 1)\n");
	printf("  89             AG peak used for alignement\n");
	printf("Error handling:\n");
	printf("  word_align: main.cpp:XXX: int main(int, char**): Assertion `startAGR' failed.\n");
	printf("  Choose a lower/an even lower value for AG_DS_PEAK. Look for threshold().\n");
}


int main (int argc, char *argv[]) {

	char *filename_AG = NULL;
	char *filename_WD = NULL;
	int num_WD = 0;
	char *filename_AGWD = NULL;
	char *filename_ALN = NULL;
	float thresholdWD = 0.00;
	unsigned int p0AG = 0;
	unsigned int p0WD = 0;

	switch (argc) {
		case 2:
			if (strcmp(argv [1], "--help") == 0) {
				welcome();
				help();
				return 0;
			}
			break;
		case 17:
			if (strcmp(argv [1], "--sequence") == 0 &&
					strcmp(argv [3], "--wd") == 0 &&
					strcmp(argv [5], "--num") == 0 &&
					strcmp(argv [7], "--out") == 0 &&
					strcmp(argv [9], "--log") == 0 &&
					strcmp(argv [11], "--th") == 0 &&
					strcmp(argv [13], "--pwd") == 0 &&
					strcmp(argv [15], "--pag") == 0) {
				filename_AG = argv[2];
				filename_WD = argv[4];
				assert(sscanf(argv [6], "%d", &num_WD) == 1);
				filename_AGWD = argv[8];
				filename_ALN = argv[10];
				assert(sscanf(argv [12], "%f", &thresholdWD) == 1);
				assert(sscanf(argv [14], "%d", &p0WD) == 1);
				assert(sscanf(argv [16], "%d", &p0AG) == 1);
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

	/* Here I create the PCM objects, crucial for
	 * PCM data import.
	 * pcmWD contains the WD data (extracted from the US stream)
	 * pcmAG contains the AG sequence, as shipped by the AG500 machine from hell
	 */
	mObjectPCM<int16_t> *pcmWD;
	mObjectPCM<int16_t> *pcmAG;
	pcmWD = new mObjectPCM<int16_t>(MPCM_STEREO, MPCM_STREAM_TINY);
	pcmAG = new mObjectPCM<int16_t>(MPCM_MONO,   MPCM_STREAM_TINY);
	
	/* Read the previoulsy described WAV files */
	mAudio2PCM(filename_WD, 0, 0, pcmWD);
	mAudio2PCM(filename_AG, 0, 0, pcmAG);
	
	/* Free unused memory */
	pcmWD->Trim();
	pcmAG->Trim();

	/* Here I dump the PCM informations in handy buffers 
	 * bufferWD contains the WD data, right channel of the US stream
	 * bufferSN contains the SN (sync) data, left channel of the US stream 
	 * bufferAG contains the AG SEQ data. This buffer will be used r/w 
	 *          for peak detection. The original information is gonna
	 *          be lost. In fact...
	 * bufferAG_copy ...contains a copy of the AG SEQ data
	 */
	int16_t *bufferWD = NULL;
	int16_t *bufferSN = NULL;
	int16_t *bufferAG = NULL;
	int16_t *bufferAG_copy = NULL;

	/* A size for almost each buffer. Almost each means
	 * that bufferWD and bufferSN have the same size samplesWD.
	 * Also bufferAG and its copy bufferAG_copy have the same size,
	 * but they are used for processing, so I prefere to keep two 
	 * sizes.
	 */
	unsigned int samplesWD;
	unsigned int samplesAG;	
	unsigned int samplesAG_copy;	

	/* Fill the buffers... */
	pcmWD->ExportBuffers(&bufferSN, &bufferWD, samplesWD);
	pcmAG->ExportBuffer(&bufferAG, samplesAG);
	pcmAG->ExportBuffer(&bufferAG_copy, samplesAG_copy);
	
	/* ... and release the unused memory. */
	delete pcmAG;
	delete pcmWD;
	free(bufferSN);
	
	/* Threshold the WD and AG buffers.
	 * AG buffer thresholding uses a standard value
	 * WD buffer requires the same value used by pdetect2.
	 * The value is stored in config/lm_pdetect.cfg
	 */
	threshold(bufferWD, samplesWD, thresholdWD);
	threshold(bufferAG, samplesAG, AG_TH);

#ifdef DEVELOP
	/* Now is time to look at the thresholded signals */
	mEncodePCM16("debug/th_wd.wav", bufferWD, NULL, samplesWD, 48000, 1);
	mEncodePCM16("debug/th_ag.wav", bufferAG, NULL, samplesAG, 16000, 1);
#endif
	
	/* Remove spikes and artifacts. 
	 * clean_spurious_spikes runs twice, just because I'm used 
	 * to work on the clean_spikes code. It's a preventive bug-fix.
	 */
	unsigned int peaksWD;

	clean_spurious_spikes(bufferWD, samplesWD);
	peaksWD = clean_spikes(bufferWD, samplesWD, US_DS_PEAK, US_DS_ARTI);
	clean_spurious_spikes(bufferWD, samplesWD);

	unsigned int peaksAG;
	clean_spurious_spikes(bufferAG, samplesAG);
	//peaksAG = clean_spikes(bufferAG, samplesAG, AG_DS_PEAK, AG_DS_ARTI);
	peaksAG = clean_spikes(bufferAG, samplesAG, 10, AG_DS_ARTI);
	clean_spurious_spikes(bufferAG, samplesAG);
	
	printf("Peaks found on RAW data:\n");
	printf("     WD data:  %d\n", peaksWD);
	printf("     AG data: %d\n", peaksAG);
	
#ifdef DEVELOP
	/* Now is time to look at those spiky signals */
	mEncodePCM16("debug/p_wd.wav", bufferWD, NULL, samplesWD, 48000, 1);
	mEncodePCM16("debug/p_ag.wav", bufferAG, NULL, samplesAG, 16000, 1);
#endif

	/* Sample-and-Hold AG signal (the spiky one) 
	 * This method works fine with spiky signals since
	 * it samples and holds a sample value for samplesAGR
	 * times.
	 * Q: Why 3?
	 * A: AG = 16kHz, WD = 48kHz. I want to compare the WD spiky
	 * signal with the resampled version or the AG spiky signal, find
	 * the WD peaks there, calculate the lag, crop the AG signal rescaling
	 * bach the AGR sample values.
	 */
	unsigned int samplesAGR = 3 * samplesAG;
	int16_t *bufferAGR = (int16_t *)malloc(samplesAGR * sizeof(int16_t));
	memset(bufferAGR, 0, samplesAGR * sizeof(int16_t));
	
	resample_linear(bufferAG, bufferAGR, samplesAG, 3, 0);
	
#ifdef DEVELOP
	/* Now is time to look at the spiky signal, resampled. */
	mEncodePCM16("debug/p_agr.wav", bufferAGR, NULL, samplesAGR, 48000, 1);
#endif

	/* Collect the peaks found on the AGR data */
	WD_peaks peaks_dataAGR;
	memset(&peaks_dataAGR, 0, sizeof(WD_peaks));
	find_peaks(bufferAGR, samplesAGR, peaks_dataAGR, 0);
	
	/* Collect the peaks found on the WD data */
	WD_peaks peaks_dataWD;
	memset(&peaks_dataWD, 0, sizeof(WD_peaks));
	find_peaks(bufferWD, samplesWD, peaks_dataWD, 0);
	
	printf("Peaks found on cleaned data:\n");
	printf("     WD data:  %d\n", peaks_dataWD.tot);
	printf("     AGR data: %d\n", peaks_dataAGR.tot);
	
#ifdef DEVELOP
	printf("Peaks AGR:\n");
	print_peaks(peaks_dataAGR);
	
	printf("Peaks WD:\n");
	print_peaks(peaks_dataWD);
#endif

	/* Sequence peak-consistency check.
	 * Controls if the sequence starts with 3 +SAT peaks 
	 * and if it ends with 3 -SAT peaks.
	 */
	bool start_ok = check_sequence_start(peaks_dataAGR);
	bool stop_ok  = check_sequence_stop(peaks_dataAGR);
	printf("AGR sequence peak-consistency:\n");
	printf("     Start: %s\n", start_ok ? "passed" : "failed");
	printf("     Stop:  %s\n", stop_ok ? "passed" : "failed");

	/* Two cases here:
	 * - If we are aligning using the WD start peak, it means
	 *   that the WD stop peak is/is not distorted/broken/ghosted,
	 *   so the WD start peak will be the first one in 
	 *   peaks_dataWD.
	 * - If we are aligning using the WD stop peak, it means
	 *   that the WD start peak is broken. 
	 *   Just one peak should be present in peaks_dataWD 
	 * So, p0WD_idx is alwais equal to 0!
	 */
	unsigned int p0WD_idx = 0;
	printf("Peaks used for the alignment:\n");
	printf("     AGR: %d (value=%d, lenght=%d)\n",
			p0AG, peaks_dataAGR.type[p0AG], peaks_dataAGR.length[p0AG]);
	printf("     WD:  %d (value=%d, lenght=%d)\n", 
			p0WD, peaks_dataWD.type[p0WD_idx], peaks_dataWD.length[p0WD_idx]);

	/* Let's use AGR as reference and find where the WD 
	 * word is, calculating the lag between the two signals.
	 */
	unsigned int s0AGR = peaks_dataAGR.start[p0AG] - peaks_dataWD.start[p0WD_idx];
	unsigned int s1AGR = s0AGR + samplesWD;

	if(p0WD == 0)
		assert(peaks_dataWD.tot == 2);
	else if(p0WD == 1)
		assert(peaks_dataWD.tot == 1);

	/*
	unsigned int s0AGR = 0;
	unsigned int s1AGR = 0;
	if(p0WD == 0) {
		s0AGR = peaks_dataAGR.start[p0AG] - peaks_dataWD.start[p0WD];
		s1AGR = s0AGR + samplesWD;
	}
	else {
		s0AGR = peaks_dataAGR.start[p0AG] - peaks_dataWD.start[p0WD_idx];
		s1AGR = s0AGR + samplesWD;
	}
	*/

	/* Scale the AGR samples values back to 16kHz (for AG)... */
	unsigned int s0AG = s0AGR/3;
	unsigned int s1AG = s1AGR/3;
	/* ...and to 200Hz for AGAMP/POS. */
	unsigned int s0AGAMP = s0AG/80;
	unsigned int s1AGAMP = s1AG/80;

	/* Done! As usual, put spam on the term... */
	printf("Results, in full ALN fashion:\n");
	printf("     AGR:   %d:%d\n", s0AGR, s1AGR); 
	printf("     AG:    %d:%d\n", s0AG,  s1AG); 
	printf("     AGAMP: %d:%d\n", s0AGAMP, s1AGAMP); 

	/* I'm ready to save a cropped version of the AG SEQ file, that
	 * matches in lenght the WD word.
	 * Here I crop...
	 */
	unsigned int samplesAGWD = s1AG - s0AG + 1;
	int16_t *bufferAGWD = (int16_t *)malloc(samplesAGWD * sizeof(int16_t));

	/* .. and here I put ham on some audio file... */
	memcpy(bufferAGWD, bufferAG_copy + s0AG, samplesAGWD * sizeof(int16_t));
	mEncodePCM16(filename_AGWD, bufferAGWD, NULL, samplesAGWD, 16000, 1);

	/* .. and spam on some ALN file. */
	FILE *file_align = fopen(filename_ALN, "w");
	fprintf(file_align, "%d/%d/", s0AGR, s1AGR);
	fprintf(file_align, "%d/%d/", s0AG, s1AG);
	fprintf(file_align, "%d/%d\n", s0AGAMP, s1AGAMP);
	fclose(file_align);
	
	/* Cleaning up memory */
	free(bufferWD);
	free(bufferAG);
	free(bufferAG_copy);
	free(bufferAGR);

	return 0;
}
