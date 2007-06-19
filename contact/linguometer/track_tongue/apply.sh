
#root="/mnt/contact_cache"
#bases="exp_0004/seq_0000 exp_0006/seq_0000 exp_0004/seq_0000"

root="/scratch/linguometer"
bases="seq_0004"

# 4 6 8

for b in $bases; do
    base="$root/$b"


    for f in `cd $base; ls *cc.dv`; do
	./track_mouth --file config2.ini --source $base/$f || exit 1
	n=`basename $f .dv`
	cp movie.avi $base/$n.cc.avi
	cp lips.txt $base/$n.lips.txt
    done

done
