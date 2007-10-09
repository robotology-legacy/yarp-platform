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

function [result_b result_v] = lmpkgCheckFiles (file_v)

result_v = {};
result_b = 1;

for fn = 1:length(file_v)
	result_v{fn} = exist(file_v{fn}, 'file');
	printf('[lmpkgCheckFiles] File %s, status %d\n', file_v{fn}, result_v{fn});
end

for r = 1:length(result_v)
	result_b = result_b & result_v{r};
end
