/* Coded by M. Tavella
 * email michele@liralab.it
 * web   http://mt.homelinux.org 
 */
#ifndef MPEAKTOOLS_H
#define MPEAKTOOLS_H

#include "mInclude.h"

#define WD_MAX 256
struct WD_peaks {
	unsigned int tot;
	unsigned int start  [WD_MAX];	/* Peak start (in samples) */
	unsigned int stop   [WD_MAX];	/* Peak stop (in samples) */
	unsigned int length [WD_MAX];	/* Peak length (in samples) */
	int16_t 	 type   [WD_MAX];	/* Peak type (+/- SAT, 0 means null) */
};


struct WD_word {
	unsigned int idx;
	unsigned int p0;			/* Peak: start */
	unsigned int p1;			/* Peak: stop */
	unsigned int s0;			/* Word start (in samples) */
	unsigned int s1;			/* Word stop (in samples) */
	unsigned int length;		/* Word length (in samples) */
};


/* The +/- SAT signals were designed to work
 * well at 48kHz.
 * AG speech signals are sampled at 16kHz.
 * A peak is a peak if:
 *   AG_DS_PEAK < ds < AG_DS_ARTI
 * where ds is the peak duration in samples.
 */
#define AG_DS_PEAK 20
#define AG_DS_ARTI 40

#define US_DS_PEAK 40
#define US_DS_ARTI 80

#define AG_TH 0.60 
#define US_TH 0.95


void threshold(int16_t *buffer, unsigned int samples, double th) {
	for(unsigned int s = 0; s < samples; s++) 
		if(buffer[s] >= th*SAT_POS)
			buffer[s] = SAT_POS;
		else if (buffer[s] <= th*SAT_NEG)
			buffer[s] = SAT_NEG;
		else
			buffer[s] = 0;
}

void negative(int16_t *buffer, unsigned int samples) {
	for(unsigned int s = 0; s < samples; s++) 
		buffer[s] = -buffer[s];
}

void clean_spurious_spikes(int16_t *buffer, unsigned int samples) {
	for(unsigned int s = 1; s < samples - 1; s++) 
		if(buffer[s] == SAT_POS) {
			if(buffer[s - 1] != SAT_POS && buffer[s + 1] != SAT_POS) 
				buffer[s] = 0;
		}
		else if(buffer[s] == SAT_NEG) {
			if(buffer[s - 1] != SAT_NEG && buffer[s + 1] != SAT_NEG)
				buffer[s] = 0;
		}
}

unsigned int clean_spikes(int16_t *buffer, unsigned int samples, 
		unsigned int ds_ll, unsigned int ds_ul)
{
	unsigned int s0;
	unsigned int sp;
	unsigned int ds;
	unsigned int s = 0;
	unsigned int peaks = 0;
	//for(unsigned int s = 0; s < samples; s++) {
	while (s < samples) {
		s0 = 0;
		sp = 0;
		ds = 0;
		if(buffer[s] == SAT_POS) {
			s0 = s;
			sp = s;
			ds = 0;
			while(buffer[sp] == SAT_POS && sp < samples)
				++sp;
		}
		else if(buffer[s] == SAT_NEG) {
			s0 = s;
			sp = s;
			ds = 0;
			while(buffer[sp] == SAT_NEG && sp < samples)
				++sp;
		}
		ds = sp - s0 + 1;
		if (ds > 1) {
			if(ds < ds_ll || ds > ds_ul) 
				for(unsigned int sc = s0; sc < sp; sc++)
					buffer[sc] = 0;
			s += ds;
			++peaks;
		}
		else
			s++;
	}
	return peaks;
}
	
unsigned int median(int16_t *buffer, unsigned int samples) {
	unsigned int s0;
	unsigned int s1;
	unsigned int ds;
	unsigned int s = 0;
	unsigned int peaks = 0;
	while (s < samples) {
	//for(unsigned int s = 0; s < samples; s++) {
		s0 = 0;
		s1 = 0;
		ds = 0;
		if(buffer[s] == SAT_POS) {
			s0 = s;
			s1 = s;
			ds = 0;
			while(buffer[s1] == SAT_POS && s1 < samples)
				++s1;
		}
		else if(buffer[s] == SAT_NEG) {
			s0 = s;
			s1 = s;
			ds = 0;
			while(buffer[s1] == SAT_NEG && s1 < samples)
				++s1;
		}
		
		ds = s1 - s0 + 1;
		if (ds > 1) {
			int16_t s_old = buffer[s];
			unsigned int ds2 = (unsigned int )floor(ds/2);
			s += ds;
			++peaks;
			for(unsigned int sm = 0; sm < ds; sm++) {
				if(sm != ds2)
					buffer[s0 + sm] = 0;
				else
					buffer[s0 + sm] = s_old;
			}
		}
		else
			s++;
	}
	return peaks;
}


unsigned int first(int16_t *buffer, unsigned int samples) {
	for(unsigned int s = 0; s < samples; s++)
		if(buffer[s] != 0)
			return s;
	return 0;
}

/* Returns the number of values at +/-SAT */
unsigned int find_peak_values(int16_t *buffer,unsigned int samples,
		unsigned int *peaks) {
	unsigned int p = 0;
	for(unsigned int s = 0; s < samples; s++)
		if(buffer[s] != 0) {
			peaks[p] = s;
			++p;
		}
	return p;
}

/* Find peaks in a sequence. Each shift_s samples are 
 * added to each peak sample value
 */
void find_peaks(int16_t *buffer,unsigned int samples, 
		WD_peaks &peaks, unsigned int shift_s) {
	unsigned int satp = 0;
	unsigned int satn = 0;
	for (unsigned int s = 0; s < samples; s++) {
		if (buffer[s] == SAT_POS) {
			if (buffer[s - 1] == 0) {
				satp = 1;
				/* +SAT peak start detected */
				peaks.start[peaks.tot] = s + shift_s;
			}
			else if (buffer[s - 1] == SAT_POS) {
				if (buffer[s + 1] == SAT_POS)
					++satp;
				else {
					//++satp;
					/* -SAT peak stop detected */
					peaks.stop[peaks.tot] = s + shift_s;
					peaks.length[peaks.tot] = satp;
					peaks.type[peaks.tot] = SAT_POS;
					peaks.tot = peaks.tot + 1;
					satp = 0;
				}
			}
		}
		else if (buffer[s] == SAT_NEG) {
			if (buffer[s - 1] == 0) {
				satn = 1;
				/* -SAT peak start detected */
				peaks.start[peaks.tot] = s + shift_s;
			}
			else if (buffer[s - 1] == SAT_NEG) {
				if (buffer[s + 1] == SAT_NEG) 
					++satn;
				else {
					//++satn;
					/* +SAT peak stop detected */
					peaks.stop[peaks.tot] = s + shift_s;
					peaks.length[peaks.tot] = satn;
					peaks.type[peaks.tot] = SAT_NEG;
					peaks.tot = peaks.tot + 1;
					satn = 0;
				}
			}
		}
	}
}


void print_peaks(WD_peaks peaks) {
	for(unsigned int p = 0; p < peaks.tot; p++) {
		printf("%.3d-%.3d/%d/%d/%d/%d\n",
				p, peaks.tot,
				peaks.start[p], peaks.stop[p], 
				peaks.length[p], peaks.type[p]); 
	}
}


bool print_peak(WD_peaks peaks, unsigned int p) {
	if(p >= 0 && p < peaks.tot) {
		printf("%.3d-%.3d/%d/%d/%d/%d\n",
				p, peaks.tot,
				peaks.start[p], peaks.stop[p], 
				peaks.length[p], peaks.type[p]);
		return true;
	}
	return false;
}


unsigned int find_front(int16_t *buffer,unsigned int samples,
		unsigned int *peaks) {
	unsigned int p = 0;
	unsigned int s = 0;
	//for(unsigned int s = 0; s < samples; s++)
	while(s < samples) {
		int16_t value = buffer[s];
		if(buffer[s] != 0) {
			peaks[p] = s;
			++p;
			do {
				++s;
			} while(buffer[s] == value);
		}
		else
			++s;
	}
	return p;
}


/* Resample a signal */
void resample_linear(int16_t *buffer_in, int16_t *buffer_out, 
		unsigned int samples_in, unsigned int samples_hold) {
	for(unsigned int s = 0; s < samples_in; s++) 
		for(unsigned int h = 0; h < samples_hold; h++)
			buffer_out[samples_hold*s + h] = buffer_in[s];
}


/* Resample a signal, leaving "zero" values untuched */
void resample_linear(int16_t *buffer_in, int16_t *buffer_out, 
		unsigned int samples_in, unsigned int samples_hold,
		int16_t zero) {
	for(unsigned int s = 0; s < samples_in; s++) 
		if(buffer_in[s] != zero)
			for(unsigned int h = 0; h < samples_hold; h++)
				buffer_out[samples_hold*s + h] = buffer_in[s];
}

/* Checks if the WD_peaks sequence starts correcly,
 * that means:
 * +SAT +SAT +SAT    +SAT              -SAT
 * <------------>    <-------------------->
 *   SEQ start            First word
 */
bool check_sequence_start(WD_peaks peaks) {
	bool start_ok = true;
	for(unsigned int p = 0; p < 4; p++) 
		start_ok &= (peaks.type[p] == SAT_POS);
	start_ok &= (peaks.type[4] == SAT_NEG);

	return start_ok;
}

/* Checks if the WD_peaks sequence stops correcly,
 * that means:
 *
 * +SAT              -SAT      -SAT -SAT -SAT    
 * <-------------------->      <------------>    
 *   Last word                   SEQ stop            
 */
bool check_sequence_stop(WD_peaks peaks) {
	bool stop_ok = true;
	for(unsigned int p = peaks.tot - 4; p < peaks.tot; p++)
		stop_ok &= (peaks.type[p] == SAT_NEG);
	stop_ok &= peaks.type[peaks.tot - 5] == SAT_POS;

	return stop_ok;
}



#endif
