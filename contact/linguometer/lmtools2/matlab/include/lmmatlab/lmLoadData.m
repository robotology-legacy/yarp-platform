function data_out = lmLoadData (AG, filename);
printf ('[lmLoadData] Loading: %s\n', filename);
data_out = lmFixData(AG, loaddata(filename));
