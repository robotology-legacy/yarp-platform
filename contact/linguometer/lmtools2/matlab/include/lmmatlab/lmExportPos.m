% Copyright (C) 2007 Michele Tavella <michele@liralab.it>
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

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
