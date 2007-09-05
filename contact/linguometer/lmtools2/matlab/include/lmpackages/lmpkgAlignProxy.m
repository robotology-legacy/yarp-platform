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
%
% function lmpkgAlignProxy(seq, num, file_data, opt_invert, xpr, txt)
function lmpkgAlignProxy(seq, num, file_data, opt_invert, xpr, txt)

data = {};
%file_data = sprintf('seq_%.4d/wd_%.4d.mat', seq, num);

data = lmpkgAlign(seq, num, 1, opt_invert, 1);

if (isempty(data) == 1)
	data = lmpkgAlign(seq, num, 1, opt_invert, 0);
end

if (isempty(data) == 0)
	data.misc.xpr = xpr;
	data.misc.seq = seq;
	data.misc.num = num;
	data.misc.txt = txt;

	printf('[lmpkgAlignProxy] Saving data: %s\n', file_data);
	save(file_data, '-struct', 'data');
else
	printf('[lmpkgAlignProxy] Could not save data: %s\n', file_data);
end

quit
