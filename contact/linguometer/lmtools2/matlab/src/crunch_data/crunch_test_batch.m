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

function crunch_test_batch (id)
%--------------------------------%
% Crunch Test Batch (SMP trick)  %
%--------------------------------%
sweeps 		= 1:3;
sensors 	= 1:12;
path_amp_AG = 'amps';
path_amp 	= 'lm_amps';
path_pos 	= 'lm_pos';

if (id == 0)
	experiments = [1:5];
else
	experiments = [6:9];
end


path_root = '~/ContactData/Experiments/experiment_NNNN/AG/Test/';

for e = experiments
	e_t = sprintf('%.4d', e);
	path_root_t = regexprep (path_root, 'NNNN', e_t);
	printf ('Current Path: %s\n', path_root_t);
	pause(2);
	
	[s1, m1, mid1] = mkdir (path_root_t, path_amp);
	[s2, m2, mid2] = mkdir (path_root_t, path_pos);

	prepdata (path_root_t, path_amp_AG, path_amp, sweeps);
	tapad    (path_root_t, path_amp,    path_pos, sweeps, sensors);  
end
