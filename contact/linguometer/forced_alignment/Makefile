
EXPERIMENT = 0009
SEQUENCE = 0005

LABELS = /mnt/contact_cache/exp_$(EXPERIMENT)/temp/ws_sequence_${SEQUENCE}.txt
WAVDIR = /mnt/contact_cache/exp_$(EXPERIMENT)/seq_${SEQUENCE}

default:
	cat $(LABELS) | ./process-sequence.pl $(WAVDIR)
	echo 
	echo "Done!"
	echo "Check for .gif files in /tmp"

img:
	./run-sphinx.sh /tmp/input.wav /tmp/input.txt
	echo "genspectrogram" | octave
	./annotate-spectrogram.pl
