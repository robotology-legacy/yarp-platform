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

function lmpkgRepackProxy(path)

num  = 0;
num_ul = 100;
file = sprintf('%s/wd_%.4d.mat', path, num);

while (1)
	file = sprintf('%s/wd_%.4d.mat', path, num);
	if (exist(file, 'file'))
		lmpkgRepack(file);
		printf('[lmpkgRepackProxy] Re-Packing %s\n', file);
		num = num + 1;
	elseif (num < num_ul)
		printf('[lmpkgRepackProxy] File %s does not exist... But I am feeling Luky (TM) since %d <= %d.\n', file, num, num_ul);
		num = num + 1;
	else
		break;
	end
end
exit;
