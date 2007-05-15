function lmExportAmp (file_amp, file_txt)
% ---------------------------------------------------------------------------- %
printf('[lmExportAmp] Reading file: %s\n', file_amp);
printf('[lmExportAmp] Writing file: %s\n', file_txt);

AG = lmProfile ();
data = lmLoadData(AG, file_amp);
%[file_amp file_pos] = lmDataPath(root, exper, sweep, kind, type);
%data = lmCollectData(AG, file_amp, file_pos);

fid = fopen(file_txt, 'w');
% I feel sorry for this loop:
for s = 1:data.samples
	%ch = 01; fprintf(fid, '%d %f %f %f %f %f %f ',  (s - 1), data.amp(s, 1:6, ch));
	ch = 01; fprintf(fid, '%f %f %f %f %f %f ',data.amp(s, 1:6, ch));
	ch = 02; fprintf(fid, '%f %f %f %f %f %f ',   data.amp(s, 1:6, ch));
	ch = 03; fprintf(fid, '%f %f %f %f %f %f ',   data.amp(s, 1:6, ch));
	ch = 04; fprintf(fid, '%f %f %f %f %f %f ',   data.amp(s, 1:6, ch));
	ch = 05; fprintf(fid, '%f %f %f %f %f %f ',   data.amp(s, 1:6, ch));
	ch = 06; fprintf(fid, '%f %f %f %f %f %f ',   data.amp(s, 1:6, ch));
	ch = 07; fprintf(fid, '%f %f %f %f %f %f ',   data.amp(s, 1:6, ch));
	ch = 08; fprintf(fid, '%f %f %f %f %f %f ',   data.amp(s, 1:6, ch));
	ch = 09; fprintf(fid, '%f %f %f %f %f %f ',   data.amp(s, 1:6, ch));
	ch = 10; fprintf(fid, '%f %f %f %f %f %f ',   data.amp(s, 1:6, ch));
	ch = 11; fprintf(fid, '%f %f %f %f %f %f ',   data.amp(s, 1:6, ch));
	ch = 12; fprintf(fid, '%f %f %f %f %f %f\n', data.amp(s, 1:6, ch));
end
fclose(fid);
