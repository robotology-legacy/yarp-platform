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
%function lmpkgPlotSensor(data, sensors, fig, trid);
function lmpkgPlotSensor(data, sensors, fig, trid);

idxs = [1:6:72];
gfx = {'rx' 'kx' 'bx' 'gx' 'mx' 'ro' 'ko' 'm^' 'g^' 'sr' 'sb' 'sk'};
plt = {'r' 'k' 'b' 'g' 'm' 'r' 'k' 'm' 'g' 'r' 'b' 'k'};

mtSimpleFig(fig);
if (length(sensors) == 1)
	idx = idxs(sensors);
	if (trid == 1)
		xc = data.AG.pos(:, idx + 0);
		yc = data.AG.pos(:, idx + 1);
		zc = data.AG.pos(:, idx + 2);
		plot3(xc, yc, zc, char(gfx(sensors)));
		xlabel ('x');
		ylabel ('y');
		zlabel ('z');
		axis([-1 1 -1 1 -1 1]*80);
		grid on;
	else
		xc = data.AG.pos(:, idx + 0);
		yc = data.AG.pos(:, idx + 1);
		zc = data.AG.pos(:, idx + 2);
		hold on;
		plot(xc, 'r');
		plot(yc, 'k');
		plot(zc, 'b');
		xlabel ('Samples');
		ylabel (sprintf('Sensor %d', sensors));
		axis tight;
		grid on;
	end
else
	if (trid == 1)
		hold on;
		for sensor = sensors
			idx = idxs(sensor);
			xc = data.AG.pos(:, idx + 0);
			yc = data.AG.pos(:, idx + 1);
			zc = data.AG.pos(:, idx + 2);
			plot3(xc, yc, zc, char(gfx(sensor)));
		end
		hold off;
		xlabel ('x');
		ylabel ('y');
		zlabel ('z');
		axis([-1 1 -1 1 -1 1]*50);
		grid on;
	elseif (trid == 0)
		for sensor = sensors
			idx = idxs(sensor);
			subplot(4, 3, sensor);
			xc = data.AG.pos(:, idx + 0);
			yc = data.AG.pos(:, idx + 1);
			zc = data.AG.pos(:, idx + 2);
			hold on;
			plot(data.misc.time, xc, 'r');
			plot(data.misc.time, yc, 'k');
			plot(data.misc.time, zc, 'b');
			hold off;
			%xlabel ('samples');
			ylabel (sprintf('Sensor %d', sensor));
			axis tight;
			grid on;
			if (sensor == 1)
				legend ('x', 'y', 'z');
			end
			if (sensor > 9)
				xlabel('Time [s]');
			end
		end
	elseif (trid == 2)
		if (min(size(data.AG.pos)) < 72)
			L = min(size(data.AG.pos));
		else
			L = length(data.AG.pos);
		end

		for sbp = 1:4
			subplot(2, 2, sbp);
			for sensor = sensors
				idx = idxs(sensor);
				xc = data.AG.pos(:, idx + 0);
				yc = data.AG.pos(:, idx + 1);
				zc = data.AG.pos(:, idx + 2);
				hold on;
				switch (sbp)
					case 1
						plot(yc, zc, char(plt(sensor)));
						plot(yc(L), zc(L), char(gfx(sensor)), 'LineWidth', 2);
					case 2
						plot(xc, yc, char(plt(sensor)));
						plot(xc(L), yc(L), char(gfx(sensor)), 'LineWidth', 2);
					case 3
						plot(xc, zc, char(plt(sensor)));
						plot(xc(L), zc(L), char(gfx(sensor)), 'LineWidth', 2);
					case 4
						plot3(xc, yc, zc, char(plt(sensor)));
						plot3(xc(L), yc(L), zc(L), char(gfx(sensor)), 'LineWidth', 2);
				end
				hold off;

				if (sensor == sensors(length(sensors)))
					switch (sbp)
						case 1
							title('Axial view');
							xlabel('y');
							ylabel('z');
							axis equal
							grid on;
						case 2
							title('Sagittal view');
							xlabel('x');
							ylabel('y');
							axis equal
							grid on;
						case 3
							title('Coronal view');
							xlabel('x');
							ylabel('z');
							axis equal
							grid on;
						case 4
							title('Tridimensional view');
							xlabel('x');
							ylabel('y');
							zlabel('z');
							view(-30,50)
							axis equal
							grid on;
					end
				end
			end
		end
	end
end
