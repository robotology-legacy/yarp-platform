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

function lmPlotPOS(AG, data, f);

%f_array = [1 4 7 10 2 5 8 11 3 6 9 12];
%f_array = [1 4 7 10 2 5 8 11 3 6 9 12];
mtSimpleFig(f);

for i = 1:12
	subplot(3, 4, i);
	plot(data.t, data.pos(:, 1:3, i));
	grid on;
	axis ([data.t0 data.t1 -200 200]);
	f_title = sprintf('Sensor %d', i);
	title(f_title);

	if(i == 1)
		legend('x', 'y', 'z');
	end
end
