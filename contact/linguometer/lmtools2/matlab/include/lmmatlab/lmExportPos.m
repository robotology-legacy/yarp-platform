function lmExportPos (file_pos, file_txt)
% ---------------------------------------------------------------------------- %
printf('[lmExportPos] Reading file: %s\n', file_pos);
printf('[lmExportPos] Writing file: %s\n', file_txt);

AG = lmProfile ();
data = lmLoadData(AG, file_pos);

fid = fopen(file_txt, 'w');
% I feel sorry for this loop:
for s = 1:data.samples
	ch = 01; fprintf(fid, '%f %f %f %f %f %f ',	data.pos(s, 1:6, ch));
	ch = 02; fprintf(fid, '%f %f %f %f %f %f ', data.pos(s, 1:6, ch));
	ch = 03; fprintf(fid, '%f %f %f %f %f %f ', data.pos(s, 1:6, ch));
	ch = 04; fprintf(fid, '%f %f %f %f %f %f ', data.pos(s, 1:6, ch));
	ch = 05; fprintf(fid, '%f %f %f %f %f %f ', data.pos(s, 1:6, ch));
	ch = 06; fprintf(fid, '%f %f %f %f %f %f ', data.pos(s, 1:6, ch));
	ch = 07; fprintf(fid, '%f %f %f %f %f %f ', data.pos(s, 1:6, ch));
	ch = 08; fprintf(fid, '%f %f %f %f %f %f ', data.pos(s, 1:6, ch));
	ch = 09; fprintf(fid, '%f %f %f %f %f %f ', data.pos(s, 1:6, ch));
	ch = 10; fprintf(fid, '%f %f %f %f %f %f ', data.pos(s, 1:6, ch));
	ch = 11; fprintf(fid, '%f %f %f %f %f %f ', data.pos(s, 1:6, ch));
	ch = 12; fprintf(fid, '%f %f %f %f %f %f\n', data.pos(s, 1:6, ch));
end
fclose(fid);
