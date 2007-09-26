#!/bin/bash
rm -f result/*
rm -f frames/*
DIR_US="temp/us/";
DIR_CC="temp/cc/";

for i in `ls -1 ag/`;
	do  
		echo "[make] Processing $i";
		convert -quality 100 -geometry x424 $DIR_US/$i $DIR_US/$i
		convert -quality 100 -geometry x424 $DIR_CC/$i $DIR_CC/$i
		montage -tile 3x -background black -geometry +0+0 cc/$i us/$i ag/$i result/frame_$i 
		convert -geometry 1352x424 result/frame_$i result/frame_$i
		convert  result/frame_$i $(echo "frames/frame_$i" | sed -e s/png/jpg/g)
done
#mencoder "mf://frames/*.jpg" -mf fps=25 -o result.mpg -of mpeg -ovc lavc -lavcopts vcodec=mpeg2video -oac pcm -audiofile audio_dump.wav
#-lavcopts vcodec=mpeg2video
#mencoder "mf://frames/*.jpg" -mf fps=25 -o result.mpeg -ovc lavc  -oac pcm -audiofile audio_dump.wav
