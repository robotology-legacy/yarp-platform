
#root="/mnt/contact_cache"
#bases="exp_0004/seq_0000 exp_0006/seq_0000 exp_0004/seq_0000"

root="/scratch/linguometer"
bases="seq_0005"

# 4 6 8

for b in $bases; do
    base="$root/$b"


    for f in `cd $base; ls *us.dv | head -n2`; do
	rm -f path*.txt
	rm -f path*.png
	cp $base/$f in.dv
	./track_tongue --file config2.ini --verbose --source $base/$f || exit 1
	n=`basename $f .dv`
	cp movie.avi $base/$n.us.avi
	mkdir -p $base/$n.path
	cp path_00*.txt $base/$n.path
    done

done
