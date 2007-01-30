
# Just a sketch of useful commands to merge audio back into a video stream.
# This process really kills quality - it is just for making demos.
# assumes input avis are in INPUT directory, and output avis (made by
# tongue_tracker) are in OUTPUT directory with same name.
# Output produced in /tmp

INPUT=/scratch/tongue/all
OUTPUT=/scratch/tongue/samples
WORK=/tmp

for f in `cd $INPUT; ls *.avi`; do
    echo $f
    ffmpeg -i $INPUT/$f foo.wav
    sox foo.wav -c 1 foo2.wav pick -r
    mencoder -audiofile foo2.wav -o foo.avi -oac copy -ovc lavc -lavcopts vcodec=msmpeg4v2 $OUTPUT/$f
    mv foo.avi $WORK/$f
    rm -f foo.wav foo2.wav
done
