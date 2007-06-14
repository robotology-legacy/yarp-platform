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

function plot_pos_3dc (AG, data, sweep, sensors, bfig);


data_max = max(max(max(data.pos)));
data_min = min(min(min(data.pos)));

mtSimpleFig (bfig);
hold on;
for sensor = sensors
	X = data.pos(:, 1, sensor);		
	Y = data.pos(:, 2, sensor);		
	Z = data.pos(:, 3, sensor);
	
	string_style = cell2mat(AG.sst(sensor));
	plot3(X, Y, Z, string_style);
end
hold off;
axis([-200 200 -200 200 -200 200]);
grid on;
xlabel('x');
ylabel('y');
zlabel('z');

string_title = sprintf ('POS (cluster view)\nSweep %d', sweep);
title (string_title);

%path_images = ['temp/images_' file_n];
%mtExportAll(path_images, 'png', '150');
%view (0, 90)
view (-29, 18)
axis square 
