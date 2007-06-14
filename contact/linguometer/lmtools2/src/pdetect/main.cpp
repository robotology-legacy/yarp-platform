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
#include "mObjectIMG.h"
#include "mInclude.h"
#include "mPeakTools.h"

#define WD_PEAK_TH		64 + 2

#ifdef USE_CONTACT_CACHE
#	define FILE_US_STREAM	"/home/michele/ContactCache/experiment_0003/US/stream.dv"
#	define FILE_PEAKS		"/home/michele/ContactCache/peaks.wav"
#else
#	define FILE_US_STREAM	"/home/michele/Temp/cache/stream.dv"
#	define FILE_PEAKS		"/home/michele/Temp/cache/peaks.wav"
#endif

#define FILE_US_PCM		"/home/michele/ContactCache/pcm_out.wav"

#define GHOST_PEAK_DS_MIN 	38400
#define GHOST_PEAK_DS_MAX 	48000


/* According to the Cartens', the AG500 control pc starts/stops recording
 * data from the 12 sensors as soon as a thread detects that the sync
 * signal reaches saturation.
 * detect_AG_segment works on a restricted time-domain, that is a part 
 * of the US stream.
 * Now, the function simply looks for the first +SAT and -SAT values 
 * in the left channel of the decoded US stream.
 * Samples values are stored in s0 and s1.
 */
void detect_AG_segment(mObjectPCM<int16_t> *pcm, int &s0, int &s1) {
	s0 = -1;
	s1 = -1;
	for(unsigned int s = 0; s < pcm->getSamples(); s++) {
		if(pcm->getL(s) < AG_TH*SAT_NEG && s0 == -1)
		//if(pcm->getL(s) < US_TH*SAT_NEG && s0 == -1)
			s0 = s;
		else if(pcm->getL(s) > AG_TH*SAT_POS && s1 == -1) 
		//else if(pcm->getL(s) > US_TH*SAT_POS && s1 == -1) 
			s1 = s;
		if(s0 > -1 && s1 > -1)
			break;
	}
	int64_t pts0 = mObjectDV2Tools::samples2pts(s0, 48000);
	int64_t pts1 = mObjectDV2Tools::samples2pts(s1, 48000);
	cout << mObjectDV2Tools::samples2pts(s1, 48000) << endl;
	cout << "AG Segment Detected" << endl;
	cout << " Samples " << s0   << "-->" << s1   << endl; 
	cout << " PTS     " << pts0 << "-->" << pts1 << endl; 
	
	/*
	unsigned int samples = s1 - s0 + 1;
	int16_t *bufferR = (int16_t *)malloc(sizeof(int16_t) * samples);
	int16_t *bufferL = (int16_t *)malloc(sizeof(int16_t) * samples);
	for (int s = s0; s < s1; s++) {
		bufferL[s - s0] = pcm->getL(s);
		bufferR[s - s0] = pcm->getR(s);
	}
	mEncodePCM16("/home/michele/test.wav", bufferL, bufferR, samples, 48000, 2);
	mEncodePCM16("/home/michele/test2.wav", bufferL, bufferR, samples, 48000, 2);
	free (bufferR);
	free (bufferL);
	*/
}


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


/* This method performs a binary treshold on the data and generates
 * a uint16_t vector of peaks.
 * The main idea here is to detect where the peaks are located and then
 * to segment the data with ffmpeg.
 */
void detect_WD_peaks (mObjectPCM<int16_t> *pcm, int s0, int s1, WD_peaks &peaksd, unsigned int S0) {
	
	memset(&peaksd, 0, sizeof(WD_peaks));

	unsigned int samples = s1 - s0 + 1;
	
	/* Create  the peaks vector and memset it to 0 */
	int16_t *peaks = (int16_t *)malloc(sizeof(int16_t) * samples);
	memset(peaks, 0, sizeof(int16_t) * samples);


	/* Peak detection: threshold on values. */
	for (int s = s0; s < s1; s++) {
		if (pcm->getR(s) > AG_TH*SAT_POS)
		//if (pcm->getR(s) > US_TH*SAT_POS) last
			peaks[s - s0] = SAT_POS;
		//else if (pcm->getR(s) < 0.20*SAT_NEG)
		//if (pcm->getR(s) < AG_TH*SAT_NEG) last
		else if (pcm->getR(s) < AG_TH*SAT_NEG)
			peaks[s - s0] = SAT_NEG;
	}
	
	/* Perform a dummy peak search. */
	unsigned int satp = 0;
	unsigned int satn = 0;
	for (unsigned int s = 0; s < samples; s++) {
		if (peaks[s] == SAT_POS) {
			if (peaks[s - 1] == 0) {
				satp = 1;
				/* +SAT peak start detected */
				peaksd.start[peaksd.tot] = s + S0;
			}
			else if (peaks[s - 1] == SAT_POS) {
				if (peaks[s + 1] == SAT_POS) {
					++satp;
				}
				else {
					 /* If peak is consistent and 
					  * not a spurious spiky spike, accept the 
					  * peak
					  */
					/*
					   if (satp >= 20) {
					   if (satp >= 20 && satp < US_DS_ARTI) {
					   if (satp > 1) {
					 */
					if (satp >= US_DS_PEAK && satp < US_DS_ARTI) {
						/* Unsure about this 
						   ++satp;
						*/
						
						/* /GHOST PEAKS DETECTION */
						unsigned int this_peak = peaksd.tot;
						unsigned int prev_peak = peaksd.tot - 1;
						/* This peak is +SAT. 
						 * If the previous peak is +SAT, two 
						 * things may be appened:
						 * 1. We are in the middle of the 3 peaks at
						 *    the beginning of each sequence
						 * 2. Maybe we skipped a -SAT ghost-peak.
						 *    Ghost-peaks exists since the cable 
						 *    that did bring the sync singnal
						 *    did wear out and sometimes it moved,
						 *    resulting in VERY SHORT peaks at
						 *    VERY LOW amplitudes. 
						 *    Cheap cables definetely suck (I 
						 *    was supposed to be out right now).
						 */
						//bool peak_is_ghost = false;
						if (peaksd.type[prev_peak] == SAT_POS) {
							//cout << "ghost peak, bitch! ";
							//cout << "at sample " << s + S0 << " 
							//with ptot = " << peaksd.tot << "\n";
							unsigned int delta_samples =
								peaksd.start[this_peak] - peaksd.stop[prev_peak];

							/* If the previous peak did end less
							 * than GHOST_PEAK_DS samples before
							 * this peak start, well, that's not a 
							 * ghost peak!
							 */
							if ((peaksd.tot <= 3 &&
									peaksd.type[prev_peak - 1] == SAT_POS && 
									peaksd.type[prev_peak - 2] == SAT_POS) || 
									delta_samples < GHOST_PEAK_DS_MIN) {
								//printf("--> WD-Start Peak detected: peak %d when dealing with +SAT-end at sample %d\n", 
								//		peaksd.tot, S0 + s);
								printf("--> WD-Start Peak detected (+SAT):\n");
								printf("    Current peak:     %d\n", this_peak);
								printf("    WD-Start peak:    %d\n", prev_peak);
								printf("    Current position: %d\n", S0 + s);
							}
							/* If the previous condition is not validated,
							 * look for the ghost peak.
							 */
							else {
								printf("--> Ghost Peak detected (+SAT):\n");
								printf("    Current peak:     %d\n", this_peak);
								printf("    WD-Start peak:    %d\n", prev_peak);
								printf("    Current position: %d\n", S0 + s);
								printf("    Ghost-recovery:   %d --> %d samples\n", S0 + s - delta_samples, S0 + s);
								
								/*
								for (unsigned int sp = S0 + s -delta_samples; sp < S0 + s; sp++) {
									//if (pcm->getR(s + s0) > 0.1*SAT_POS)
									//	peaks[s] = SAT_POS;
									if (pcm->getR(sp + s0) < 0.1*SAT_NEG)
										peaks[sp] = SAT_NEG;
								}
								peak_is_ghost = true;
								s -= S0 - delta_samples;
								*/
							}
						}
						/* GHOST PEAKS DETECTION  */

						//if (!peak_is_ghost) {
						/* -SAT peak stop detected */
						peaksd.stop[peaksd.tot] = s + S0;
						peaksd.length[peaksd.tot] = satp;
						peaksd.type[peaksd.tot] = SAT_POS;
						peaksd.tot = peaksd.tot + 1;
						//}
						satp = 0;
					}
					else {
						cout << "Dropping +peak: length " << satp;
						cout << " at sample " << s << endl;
						satp = 0;
					}
				}
			}
		}
		else if (peaks[s] == SAT_NEG) {
			if (peaks[s - 1] == 0) {
				satn = 1;
				/* -SAT peak start detected */
				peaksd.start[peaksd.tot] = s + S0;
			}
			else if (peaks[s - 1] == SAT_NEG) {
				if (peaks[s + 1] == SAT_NEG)
					++satn;
				else {
					 /* If peak is consistent and 
					  * not a spurious spiky spike, accept the 
					  * peak
					  */
					/*
					   if (satn > 1) {
					   if (satn >= 20) {
					   if (satn >= 20 && satn < US_DS_ARTI) {
					 */
					if (satn >= US_DS_PEAK && satn < US_DS_ARTI) {
						/* Unsure about this 
						   ++satn;
						*/
						
						/* +SAT peak stop detected */
						peaksd.stop[peaksd.tot] = s + S0;
						peaksd.length[peaksd.tot] = satn;
						peaksd.type[peaksd.tot] = SAT_NEG;
						peaksd.tot = peaksd.tot + 1;
						satn = 0;
					}
					else {
						cout << "Dropping -peak: length " << satn;
						cout << " at sample " << s << endl;
						satn = 0;
					}
				}
			}
		}
	}
	for (unsigned int p = 0; p < peaksd.tot; p++) {
		if (peaksd.length[p] > WD_PEAK_TH) {
			peaksd.type[p] = 0;
			printf("Peak %3d/%3d exceded max WD lenght (%d>%d)\n",
					p, peaksd.tot, 
					peaksd.length[p], WD_PEAK_TH);
		}
	}

	for (unsigned int p = 0; p < peaksd.tot; p++) {
		printf("Peak %3d/%3d, start %8d, stop %8d, length %4d, type %8d\n",
				p, peaksd.tot, peaksd.start[p], peaksd.stop[p], 
				peaksd.length[p], 
				//peaksd.stop[p] - peaksd.start[p] + 1, 
				peaksd.type[p]);
	}
	//mEncodePCM16(, peaks, NULL, samples, 48000, 1);
}	


void cook_dd_params (unsigned int s0, unsigned int sw0, unsigned int sw1,
	int64_t &f0, int64_t &f1) {
	unsigned int sf    = 48000;
	unsigned int vfPTS = 40000;
	int64_t p0 = mObjectDV2Tools::samples2pts(s0 + sw0, sf);
	int64_t p1 = mObjectDV2Tools::samples2pts(s0 + sw1, sf);
	
#ifdef DEBUG
	printf("sw0:      %d\n", sw0);
	printf("sw1:      %d\n", sw1);
	printf("PTS_sw0:  %d\n", p0);
	printf("PTS_sw1:  %d\n", p1);
	printf("SEC_sw0:  %.3f\n", (double)p0/AV_TIME_BASE);
	printf("SEC_sw1:  %.3f\n", (double)p1/AV_TIME_BASE);
#endif

	f0 = p0/vfPTS;
	f1 = p1/vfPTS;
	int64_t f0r = p0%vfPTS;
	int64_t f1r = p1%vfPTS;
	
	if (f0r > 40000/2)
		--f0;
	if (f1r > 40000/2)
		++f1;

#ifdef DEBUG
	cout << "Video Sync0 at "; 
	cout << f0;
	cout << " with error "; 
	cout << f0r << endl;
	cout << "Video Sync1 at "; 
	cout << f1;
	cout << " with error "; 
	cout << f1r << endl;

	cout << "Ffmpeg0: "; 
	cout << (double)f0/25 << endl;
	cout << "Ffmpeg0: "; 
	cout << (double)f1/25 << endl;
	cout << "FfmpegD: ";
	cout << (double)(f1 - f0)/25 << endl;

	cout << "Ffmpeg t0: "; 
	mObjectDV2Tools::pts2time(p0);
	cout << endl; 
	cout << "Ffmpeg frames: "; 
	cout << f1 - f0 << endl; 
#endif
}


int main (int argc, char *argv[]) {
	int t0 = 0, t1 = 0;
	char *file_dv = NULL, *file_log = NULL, *file_aln = NULL;
	char *file_seg = NULL, *file_seq = NULL;

	switch (argc) {
		case 2:
			if (strcmp(argv [1], "--help") == 0) {
				welcome();
				help();
				return 0;
			}
			break;
		case 15:
			if (strcmp(argv [1], "--stream") == 0 &&
					strcmp(argv [3], "--t0") == 0 &&
					strcmp(argv [5], "--t1") == 0 &&
					strcmp(argv [7], "--log") == 0 &&
					strcmp(argv [9], "--seg") == 0 &&
					strcmp(argv [11], "--seq") == 0 &&
					strcmp(argv [13], "--aln") == 0) {
				file_dv = argv [2];
				assert(sscanf(argv [4], "%d", &t0) == 1);
				assert(sscanf(argv [6], "%d", &t1) == 1);
				file_log = argv [8];
				file_seg = argv [10];
				file_seq = argv [12];
				file_aln = argv [14];
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
	int64_t AG_pts0 = mObjectDV2Tools::secs2pts(t0);
	int64_t AG_pts1 = mObjectDV2Tools::secs2pts(t1);
	unsigned int AG_s0 = mObjectDV2Tools::pts2samples(AG_pts0, 48000);

	mObjectPCM<int16_t> *pcm;
	pcm = new mObjectPCM<int16_t>(MPCM_STEREO, MPCM_STREAM_SMALL);
	
	mDV2PCM(file_dv, AG_pts0, AG_pts1, pcm);
	pcm->Trim();
	
#ifdef DEBUG_ALG
	unsigned int samples;
	int16_t *bufferR = NULL;
	int16_t *bufferL = NULL;
	pcm->ExportBuffers(&bufferL, &bufferR, samples);
	mEncodePCM16(file_seg, bufferL, bufferR, samples, 48000, 2);
	free(bufferR);
	free(bufferL);
#endif

	int s0 = 0;
	int s1 = 0;
	detect_AG_segment(pcm, s0, s1);
	assert (s0 > -1);
	assert (s1 > -1);


	FILE *FILE_ALN = fopen(file_aln, "w");
	if (FILE_ALN == NULL)
		return -1;
	fprintf(FILE_ALN, "%d\n", t0*48000 + s0);
	fclose(FILE_ALN);

#ifdef DEBUG_ALG
	bufferR = (int16_t *)malloc((s1 - s0 + 1) * sizeof(int16_t));
	for (unsigned int s = (unsigned int)s0; s < (unsigned int)s1; s++) 
		bufferR[s - s0] = pcm->getR(s);
	mEncodePCM16(file_seq, bufferR, NULL, (s1 - s0 + 1), 48000, 1);
#endif
	
	WD_peaks peaksd;
	detect_WD_peaks(pcm, s0, s1, peaksd, AG_s0);
	printf("\n\n\n");
	unsigned int words = 0;
	WD_word wordsd [WD_MAX];
	memset (wordsd, 0, WD_MAX * sizeof(WD_word));
	for (unsigned int p = 0; p < peaksd.tot; p++) {
		if (peaksd.type[p] == SAT_POS) {
			if (peaksd.type[p + 1] == SAT_NEG) {
				wordsd[words].idx = words;
				wordsd[words].p0 = p;
				wordsd[words].p1 = p + 1;
				wordsd[words].s0 = peaksd.start[p];
				wordsd[words].s1 = peaksd.stop[p + 1];
				++words;
			}
			else if (peaksd.type[p + 1] == SAT_POS &&
					peaksd.type[p + 2] == SAT_POS &&
					peaksd.type[p + 3] == SAT_NEG) {
				printf("WD-Start detected: %3d, %3d, %3d\n", 
						p, p + 1, p + 2);
			}
		}
		else if (peaksd.type[p - 1] == SAT_NEG &&
				peaksd.type[p] == SAT_NEG &&
				peaksd.type[p + 1] == SAT_NEG &&
				peaksd.type[p + 2] == SAT_NEG) {
				printf("WD-Stop detected:  %3d, %3d, %3d\n", 
						p, p + 1, p + 2);
		}
	}

	FILE *FILE_LOG = fopen(file_log, "w");
	if (FILE_LOG == NULL)
		return -1;

	int64_t f0 = 0;
	int64_t f1 = 0;
	for (unsigned int w = 0; w < words; w++) {
		cook_dd_params(s0, wordsd[w].s0 - 6000, wordsd[w].s1 + 6000,
			f0, f1);
		fprintf(FILE_LOG, "%d/%d/%d\n", w, (int)f0, (int)(f1 - f0 + 1));
	}
	fclose(FILE_LOG);
	
	
	delete pcm;
	return 0;
}






	/*
	int64_t AG_pts00 = mObjectDV2Tools::secs2pts(0);
	int64_t AG_pts01 = mObjectDV2Tools::secs2pts(250);

	int64_t pts00 = mObjectDV2Tools::secs2pts(345);
	int64_t pts01 = mObjectDV2Tools::secs2pts(535);
	
	int64_t pts00 = mObjectDV2Tools::secs2pts(565);
	int64_t pts01 = mObjectDV2Tools::secs2pts(755);
	
	int64_t pts00 = mObjectDV2Tools::secs2pts(795);
	int64_t pts01 = mObjectDV2Tools::secs2pts(985);
	
	int64_t pts00 = mObjectDV2Tools::secs2pts(1019);
	int64_t pts01 = mObjectDV2Tools::secs2pts(1174);
	
	int64_t pts00 = mObjectDV2Tools::secs2pts(1255);
	int64_t pts01 = mObjectDV2Tools::secs2pts(1402);
	
	int64_t pts00 = mObjectDV2Tools::secs2pts(1439);
	int64_t pts01 = mObjectDV2Tools::secs2pts(1631);
	
	int64_t pts00 = mObjectDV2Tools::secs2pts(1665);
	int64_t pts01 = mObjectDV2Tools::secs2pts(1861);
	
	int64_t pts00 = mObjectDV2Tools::secs2pts(1896);
	int64_t pts01 = mObjectDV2Tools::secs2pts(2085);
	*/
