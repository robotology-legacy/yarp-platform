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

function data_out = lmFilterData (data, filter_type)

if (filter_type == 0)
	load Hlp2030.mat
	filter = Hlp2030;
else
	error ('[lmFilterData] Filter Type is unknown...');
end

data_out = data;
if (isempty(data.amp) == 0)
	for coil = 1:6
		for sensor = 1:12
			%data_out.amp (:, coil, sensor) = lmpFilterVector(data.amp (:, coil, sensor), filter);
			data_out.amp (:, coil, sensor) = firfilter(data.amp (:, coil, sensor), filter);
		end
	end
end

if (isempty(data.pos) == 0)
	for dim = 1:3
		for sensor = 1:12
			%data_out.pos (:, dim, sensor) = lmpFilterVector(data.pos (:, dim, sensor), filter);
		end
	end
end


%du = data_still.amp(:, 1, 1);


function df = lmpFilterVector (du, vf)
border  = ones(length(vf), 1);
border1 = border*du(1);
border2 = border*du(length(du));
du_t = [border1; du; border2];
df_t = filter (vf, 1, du_t, [], 1);
df = df_t(length(border1) + 1:length(border1) + length(du));
	
