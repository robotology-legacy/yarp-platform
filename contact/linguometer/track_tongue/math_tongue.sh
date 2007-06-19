

root="/scratch/linguometer"
base="seq_0004"

for d in `ls -d $root/$base/*.path`; do

  (
  for f in `ls $d/path_*.txt`; do
      echo "smooth_tongue('$f');"
      sleep 1
  done
  echo "toggle_octplot;" # make sure you have "octplot" installed
  echo "summarize_tongue;"
  echo "toggle_octplot;" # make sure you have "octplot" installed
  ) | octave
  cp summary.png $d/summary.png
  convert $d/*.png $d/waggle.gif

done