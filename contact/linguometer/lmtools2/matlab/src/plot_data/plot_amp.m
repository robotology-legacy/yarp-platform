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

data_max = max(max(max(data.amp)));
data_min = min(min(min(data.amp)));

for sensor = 1:12
	mtSimpleFig(bfig + sensor);
	plot(data.t, data.amp(:, 1, sensor), 'b'); hold on;
	plot(data.t, data.amp(:, 2, sensor), 'g');
	plot(data.t, data.amp(:, 3, sensor), 'r');
	plot(data.t, data.amp(:, 4, sensor), 'c');
	plot(data.t, data.amp(:, 5, sensor), 'm');
	plot(data.t, data.amp(:, 6, sensor), 'k'); hold off;


	grid on;
	axis([data.t0 data.t1 data_min data_max]);
	
	string_title = sprintf('AMP (All coils for each sensor)\nSweep %d, Sensor %d\n', sweep, sensor);
	title(string_title);

	ylabel('Amplitudes');
	xlabel(['Time [' data.tu ']']);
	
	if (sensor == 1)
		legend ('C1', 'C2', 'C3', 'C4', 'C5', 'C6');
	end
end

%path_images = ['temp/images_' file_n];
%mtExportAll(path_images, 'png', '150');
