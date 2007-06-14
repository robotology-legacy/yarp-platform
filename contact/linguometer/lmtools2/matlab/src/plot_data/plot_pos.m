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

function plot_pos (AG, data, sweep, bfig)

data_max = max(max(max(data.pos)));
data_min = min(min(min(data.pos)));

for sensor = 1:12
	mtSimpleFig(bfig + sensor);
	plot(data.t, data.pos(:, 1, sensor), AG.dcl(1)); hold on;
	plot(data.t, data.pos(:, 2, sensor), AG.dcl(2));
	plot(data.t, data.pos(:, 3, sensor), AG.dcl(3));
	plot(data.t, data.pos(:, 4, sensor), AG.dcl(4));
	plot(data.t, data.pos(:, 5, sensor), AG.dcl(5));
	%plot(data.t, data.pos(:, 6, sensor), AG.dcl(6));
	hold off;

	grid on;
	%axis([data.t0 data.t1 data_min data_max]);
	axis([data.t0 data.t1 -200 200]);
	
	string_title = sprintf('POS (All dims for each sensos)\nSweep %d, Sensor %d\n', sweep, sensor);
	title(string_title);

	ylabel(['Space [' AG.pars.su ']']);
	xlabel(['Time [' data.tu ']']);

	if (sensor == 1)
		%legend ('x', 'y', 'z', 'phi', 'theta', 'rms');
		legend ('x', 'y', 'z', 'phi', 'theta');
	end
end

%path_images = ['temp/images_' file_n];
%mtExportAll(path_images, 'png', '150');
