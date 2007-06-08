
# working directory
WORK=tmp

WAV=$1
TXT=$2

if [ ! -e $WAV ] ; then
    print wav file $WAV does not exist
    exit 1
fi

if [ ! -e $TXT ] ; then
    print text file $TXT does not exist
    exit 1
fi

mkdir -p $WORK
cp $TXT $WORK/label.txt
cur="$WORK/current.wav"
curtxt="$WORK/current.txt"

phones=`cat $TXT | ./text2phonemes.scm | sed 's/^.\"//' | sed 's/\".*//' | tee $curtxt | ./translate-phone.pl phoneme-fest2sphinx.txt`
echo "DUMMYWORD $phones" > $WORK/current.dict
echo "DUMMYWORD" > $WORK/transcript.txt

cp $WAV $cur

sox $WORK/current.wav -c 1 -r 16000 $WORK/resampled.wav
wave2feat -mswav yes -i $WORK/resampled.wav -o $WORK/current.mfc
echo $WORK/current > $WORK/list.ctl
sphinx3_align sphinx-configuration.an4
cat $WORK/current.phseg

# Need to recover real samples from strange frame numbers sphinx uses
sox $WORK/resampled.wav -c 1 $cur.ub 2> /dev/null  # single byte per sample
samples=`wc --bytes $cur.ub | sed "s/ .*//"`
echo "$samples samples"
./reformat-phone.pl $samples < $WORK/current.phseg > $WORK/rescaled.phseg 2> $WORK/labelled.phseg
echo "========================================="
echo "== RESULT in $WORK/labelled.phseg"
cat $WORK/labelled.phseg
echo "========================================="
echo
