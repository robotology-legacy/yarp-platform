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

function out = lmpkgResample(in, factor)
size_in = size(in);
if (size_in(1) < size_in(2))
	printf('[lmpkgResample] Use colum-vectors!!!\n');
	in = in';
end
in	
samples_in = length(in)
samples_out = factor*samples_in

ds = 1;
for i = 1:factor:samples_out
	s0 = i;
	s1 = s0 + factor - 1;
	for s = s0:s1
		out(s, :) = in(ds,:);
	end
	%printf('%d: %d --> %d \n',ds, i, i+1);
	ds = ds + 1;
end

