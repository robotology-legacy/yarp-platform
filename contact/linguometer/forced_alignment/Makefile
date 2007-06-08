
EXPERIMENT = 0008

LABELS = /mnt/contact_cache/exp_$(EXPERIMENT)/temp/ws_sequence_0000.txt
WAVDIR = /mnt/contact_cache/exp_$(EXPERIMENT)/seq_0000

default:
	cat $(LABELS) | ./process-sequence.pl $(WAVDIR)
	echo 
	echo "Done!"
	echo "Check for .gif files in /tmp"

img:
	./run-sphinx.sh /tmp/input.wav /tmp/input.txt
	echo "genspectrogram" | octave
	./annotate-spectrogram.pl
