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

%--------------------------------%
% Crunch Test Sweeps             %
%--------------------------------%
sweeps 		= 1:3;
sensors 	= 1:12;
path_amp_AG = 'amps';
path_amp 	= 'lm_amps';
path_pos 	= 'lm_pos';

path_root = '~/ContactData/Experiments/current/AG/Test/';


[s1, m1, mid1] = mkdir (path_root, path_amp);
[s2, m2, mid2] = mkdir (path_root, path_pos);

prepdata (path_root, path_amp_AG, path_amp, sweeps);
tapad    (path_root, path_amp,    path_pos, sweeps, sensors);  
