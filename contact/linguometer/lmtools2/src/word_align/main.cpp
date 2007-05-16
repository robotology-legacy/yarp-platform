/* Coded by M. Tavella
 * email michele@liralab.it
 * web   http://mt.homelinux.org 
 */
#include "mObjectDV2.h"
#include "mObjectEN2.h"
#include "mObjectPCM.h"
#include "mInclude.h"
#include "mPeakTools.h"

#define DEVELOP
void help (void) {
	printf("Usage:\n");
	printf(" word_align --sequence sequence.wav --wd word_in.wav --num 10\n");
	printf("            --out word_out.wav --log file.aln\n");
	printf("Where:\n");
	printf("  sequence.wav   AG sequence (16kHz, Mono)\n");
	printf("  word_in.wav    US word  (48kHz, Stereo)\n");
	printf("  10             US word number (in sequence)\n");
	printf("  word_out.wav   AG segmented word (16kHz, Mono)\n");
	printf("  file.aln       Sync informations\n");
	printf("Error handling:\n");
	printf("  word_align: main.cpp:XXX: int main(int, char**): Assertion `startAGR' failed.\n");
	printf("  Choose a lower/an even lower value for AG_DS_PEAK. Look for threshold().\n");
}


int main (int argc, char *argv[]) {

	char *filename_AG = NULL;
	char *filename_WD = NULL;
	int num_WD = 0;
	char *filename_AGWD = NULL;
	char *filename_log = NULL;


	switch (argc) {
		case 2:
			if (strcmp(argv [1], "--help") == 0) {
				welcome();
				help();
				return 0;
			}
			break;
		case 11:
			if (strcmp(argv [1], "--sequence") == 0 &&
					strcmp(argv [3], "--wd") == 0 &&
					strcmp(argv [5], "--num") == 0 &&
					strcmp(argv [7], "--out") == 0 &&
					strcmp(argv [9], "--log") == 0) {
				filename_AG = argv[2];
				filename_WD = argv[4];
				assert(sscanf(argv [6], "%d", &num_WD) == 1);
				filename_AGWD = argv[8];
				filename_log = argv[10];
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

	mObjectPCM<int16_t> *pcmWD;
	mObjectPCM<int16_t> *pcmAG;
	pcmWD = new mObjectPCM<int16_t>(MPCM_STEREO, MPCM_STREAM_TINY);
	pcmAG = new mObjectPCM<int16_t>(MPCM_MONO,   MPCM_STREAM_TINY);
	
	/* Read WAV files */
	mAudio2PCM(filename_WD, 0, 0, pcmWD);
	mAudio2PCM(filename_AG, 0, 0, pcmAG);
	
	/* Free unused memory */
	pcmWD->Trim();
	pcmAG->Trim();

	int16_t *bufferWD = NULL;
	int16_t *bufferSN = NULL;
	int16_t *bufferAG = NULL;
	int16_t *bufferAGC = NULL;
	unsigned int samplesWD;
	unsigned int samplesAG;	
	unsigned int samplesAGC;	

	/* Get buffers */
	pcmWD->ExportBuffers(&bufferSN, &bufferWD, samplesWD);
	pcmAG->ExportBuffer(&bufferAG, samplesAG);
	pcmAG->ExportBuffer(&bufferAGC, samplesAGC);
	
	/* Not needed any more... */
	delete pcmAG;
	delete pcmWD;
	free(bufferSN);
	
	/* Threshold */
	threshold(bufferWD, samplesWD, US_TH);
	threshold(bufferWD, samplesWD, 0.50);
	//threshold(bufferAG, samplesAG, AG_TH);
	
	/* Remove spikes and artifacts */
	unsigned int peaksWD;
	unsigned int peaksAG;
	peaksWD = clean_spikes(bufferWD, samplesWD, US_DS_PEAK, US_DS_ARTI);
	/*	
	#define AG_DS_PEAK 20
	#define AG_DS_ARTI 40

	#define US_DS_PEAK 40
	#define US_DS_ARTI 80

	#define AG_TH 0.60 
	#define US_TH 0.95
	*/

	/* AG peaks are not so trivial to detect in the context of batch
	 * processing. 
	 */
	//peaksAG = clean_spikes(bufferAG, samplesAG, AG_DS_PEAK, AG_DS_ARTI);
	peaksAG = clean_spikes(bufferAG, samplesAG, 10, AG_DS_ARTI);
	
#ifdef DEVELOP
	/* Now is time to look at the signals */
	mEncodePCM16("temp/p_wd.wav", bufferWD, NULL, samplesWD, 48000, 1);
	mEncodePCM16("temp/p_ag.wav", bufferAG, NULL, samplesAG, 16000, 1);
#endif


	/* Sample-and-Hold AG signal (the spiky one) 
	 */
	unsigned int samplesAGR = 3 * samplesAG;
		
	/* Secondly, I alloc a buffer
	 */
	int16_t *bufferAGR = (int16_t *)malloc(samplesAGR * sizeof(int16_t));
	memset(bufferAGR, 0, samplesAGR * sizeof(int16_t));
	
	resample_linear(bufferAG, bufferAGR, samplesAG, 3, 0);
	
#ifdef DEVELOP
	/* Now is time to look at the resampled signal */
	mEncodePCM16("temp/p_agr.wav", bufferAGR, NULL, samplesAGR, 48000, 1);
#endif

	/* Peak detection on AGR data */
	WD_peaks peaks_dataAGR;
	memset(&peaks_dataAGR, 0, sizeof(WD_peaks));
	find_peaks(bufferAGR, samplesAGR, peaks_dataAGR);
	
	/* Peak detection on WD data */
	WD_peaks peaks_dataWD;
	memset(&peaks_dataWD, 0, sizeof(WD_peaks));
	find_peaks(bufferWD, samplesWD, peaks_dataWD);

#ifdef DEVELOP
	printf("Peaks AGR:\n");
	print_peaks(peaks_dataAGR);
	
	printf("Peaks WD:\n");
	print_peaks(peaks_dataWD);
#endif

	/* Let's check if:
	 * + the first 3 peaks are +SAT
	 * + the last 3 peaks are -SAT
	 * HUGE BUG OVER HERE! Patched May 16, 2007
	 * The test does not take in account that 
	 * the 4th value HAS TO BE -SAT, 
	 * at least for the startAGR value!
	 */
	bool startAGR = true;
	for(unsigned int p = 0; p < 3; p++)
		if(peaks_dataAGR.type[p] == SAT_POS)
			startAGR &= true;
		else
			startAGR &= false;
	/* Patch begins here */
	if(startAGR && peaks_dataAGR.type[4] == SAT_NEG)
		startAGR = true;
	else
		startAGR = false;
	/* Patch stops here */
	assert(startAGR); 


	bool stopAGR = true;
	/* Patch begins here */
	//for(unsigned int p = peaks_dataAGR.tot - 3; p < peaks_dataAGR.tot; p++)
	for(unsigned int p = peaks_dataAGR.tot - 4; p < peaks_dataAGR.tot; p++)
	/* Patch stops here */
		if(peaks_dataAGR.type[p] == SAT_NEG)
			stopAGR &= true;
		else
			stopAGR &= false;
	/* This assert is not stricly necessaire for alignment purposes.
	 * It could trow errors that will not break the lmtools/lmscritps 
	 * workflow.
	 * The lmtools  code has been written resist to poor stops 
	 * in the recording sessions. I do not really care "that much" 
	 * for the end of the sequences. 
	 * This assert require a "strict" checking on segmentation 
	 * signals. 
	 */
	//assert(stopAGR);

	/* Alignment starts here */
	unsigned int p0AG = 3 + 2*num_WD + 0;
	unsigned int p0WD = 0;

#ifdef DEVELOP
	unsigned int p1AG = 3 + 2*num_WD + 1;
	unsigned int p1WD = 1;

	print_peak(peaks_dataAGR, p0AG);
	print_peak(peaks_dataAGR, p1AG);
	print_peak(peaks_dataWD,  p0WD);
	print_peak(peaks_dataWD,  p1WD);
#endif

	/* Let's use AGR as reference and find where the WD 
	 * word is...
	 */
	unsigned int s0AGR = peaks_dataAGR.start[p0AG] - peaks_dataWD.start[p0WD];
	unsigned int s1AGR = s0AGR + samplesWD;
	
	/* ...and then scale the samples values back to 16kHz */
	unsigned int s0AG = s0AGR/3;
	unsigned int s1AG = s1AGR/3;
	unsigned int s0AG500 = s0AG/80;
	unsigned int s1AG500 = s1AG/80;

	/* Done! As usual, put spam on the term... */
	printf("AGR: %d:%d\n", s0AGR, s1AGR); 
	printf("AG : %d:%d\n", s0AG,  s1AG); 

	unsigned int samplesAGWD = s1AG - s0AG + 1;
	int16_t *bufferAGWD = (int16_t *)malloc(samplesAGWD * sizeof(int16_t));

	/* .. and ham on some audio file... */
	memcpy(bufferAGWD, bufferAGC + s0AG, samplesAGWD * sizeof(int16_t));
	mEncodePCM16(filename_AGWD, bufferAGWD, NULL, samplesAGWD, 16000, 1);

	/* .. and on some log file. */
	FILE *file_align = fopen(filename_log, "w");
	fprintf(file_align, "%d/%d/", s0AGR, s1AGR);
	fprintf(file_align, "%d/%d/", s0AG, s1AG);
	fprintf(file_align, "%d/%d\n", s0AG500, s1AG500);
	fclose(file_align);
	
	/* Cleaning up memory */
	free(bufferWD);
	free(bufferAG);
	free(bufferAGC);
	free(bufferAGR);

	return 0;
}
