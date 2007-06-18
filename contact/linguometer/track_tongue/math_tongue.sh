
(
for f in `ls path_*.txt`; do
    echo "smooth_tongue('$f');"
    sleep 1
done
) | octave
