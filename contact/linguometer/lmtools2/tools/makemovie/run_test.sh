# Usage: lm_makeonemovie usdv_file ccdv_file uswav_file agpos_file wdtxt_file /path/to/dest

ROOT="/mnt/contact_cache/exp_0004/aln_0000/";
NUM="0000";
./lm_make_1movie $ROOT/wd_"$NUM"_us.dv $ROOT/wd_"$NUM"_cc.dv $ROOT/wd_"$NUM"_us.wav $ROOT/wd_"$NUM"_ag.pos $ROOT/wd_"$NUM".txt /home/michele
