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

%function lmpkgDummy(seq, word)

%for num = [15 20]
for num = [15]
	data = load(sprintf('wd_00%d.mat', num));
	data.AG.pos = data.AG.pos(1:6*240:length(data.AG.pos),:);
	lmpkgPlotSensor(data, 1:3,   10, 2)
	lmpkgPlotSensor(data, 10:12, 11, 2)

	if(1) 
		mtSimpleFig(12);
		surf(1:1:3, data.misc.time(1:6*240:length(data.misc.time)), data.AG.pos(:,[3 9 15]));
		xlabel('Sensors 1, 2 and 3');
		ylabel('Time [s]');
		zlabel('Z-Displacement [mm]');
		%view(105,42);
		view(66, 46);

		mtPrintEPS(10, sprintf('complex_%d_tongue.eps', num));
		mtPrintEPS(11, sprintf('complex_%d_glasses.eps', num));
		mtPrintEPS(12, sprintf('complex_%d_mesh.eps', num));
	end

	if(1)
	end
end
