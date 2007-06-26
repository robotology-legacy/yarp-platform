
EXPERIMENT = 0009
SEQUENCE = 0005

OUTDIR = /scratch/linguometer/feature/exp_$(EXPERIMENT)/seq_${SEQUENCE}
LABELS = /mnt/contact_cache/exp_$(EXPERIMENT)/temp/ws_sequence_${SEQUENCE}.txt
WAVDIR = /mnt/contact_cache/exp_$(EXPERIMENT)/seq_${SEQUENCE}

OUTALL = /scratch/linguometer/all
INALL = /mnt/contact_cache

default:
	rm -f /tmp/wd*.gif || echo "no gifs lying around, good"
	cat $(LABELS) | ./process-sequence.pl $(WAVDIR)
	mkdir OUTDIR
	cp /tmp/wd*.gif $(OUTDIR)
	echo 
	echo "Done!"
	echo "Check for .gif files in /tmp or $(OUTDIR)"

img:
	./run-sphinx.sh /tmp/input.wav /tmp/input.txt
	echo "genspectrogram" | octave
	./annotate-spectrogram.pl

all:
	./looper.pl $(INALL) $(OUTALL)


