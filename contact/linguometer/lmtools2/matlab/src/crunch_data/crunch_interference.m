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
do_prep		= 1;
sensors 	= 1:12;
path_amp_AG = 'amps';
path_amp 	= 'lm_amps';
path_pos 	= 'lm_pos';

ftype 		= 'FIR2030';
path_amp_f 	= 'lm_amps_f';
path_pos_f 	= 'lm_pos_f';

if (id == 0)
	% ----------------------------------------------------------------------- %
	sweeps	= 1:7;
	if (do_prep)
		path_root = '~/ContactData/Michele/Interference/sweeps_a1/';
		[s1, m1, mid1] = mkdir (path_root, path_amp);
		[s2, m2, mid2] = mkdir (path_root, path_pos);
		prepdata (path_root, path_amp_AG, path_amp, sweeps);
		[s1, m1, mid1] = mkdir (path_root, path_amp_f);
		[s2, m2, mid2] = mkdir (path_root, path_pos_f);
		prepdata (path_root, path_amp_AG, path_amp_f, sweeps, ftype);
	else
		tapad (path_root, path_amp, path_pos, sweeps, sensors);  
		tapad (path_root, path_amp_f, path_pos_f, sweeps, sensors);  
	end
	% ----------------------------------------------------------------------- %
	sweeps	= 1:9;
	if (do_prep)
		path_root = '~/ContactData/Michele/Interference/sweeps_a2/';
		[s1, m1, mid1] = mkdir (path_root, path_amp);
		[s2, m2, mid2] = mkdir (path_root, path_pos);
		prepdata (path_root, path_amp_AG, path_amp, sweeps);
		[s1, m1, mid1] = mkdir (path_root, path_amp_f);
		[s2, m2, mid2] = mkdir (path_root, path_pos_f);
		prepdata (path_root, path_amp_AG, path_amp_f, sweeps, ftype);
	else
		tapad (path_root, path_amp, path_pos, sweeps, sensors);  
		tapad (path_root, path_amp_f, path_pos_f, sweeps, sensors);  
	end
	% ----------------------------------------------------------------------- %
	sweeps	= 1:9;
	if (do_prep)
		path_root = '~/ContactData/Michele/Interference/sweeps_a3/';
		[s1, m1, mid1] = mkdir (path_root, path_amp);
		[s2, m2, mid2] = mkdir (path_root, path_pos);
		prepdata (path_root, path_amp_AG, path_amp, sweeps);
		[s1, m1, mid1] = mkdir (path_root, path_amp_f);
		[s2, m2, mid2] = mkdir (path_root, path_pos_f);
		prepdata (path_root, path_amp_AG, path_amp_f, sweeps, ftype);
	else
		tapad (path_root, path_amp, path_pos, sweeps, sensors);  
		tapad (path_root, path_amp_f, path_pos_f, sweeps, sensors);  
	end
	% ----------------------------------------------------------------------- %
else
	sweeps	= 1:9;
	if (do_prep)
		path_root = '~/ContactData/Michele/Interference/sweeps_b1/';
		[s1, m1, mid1] = mkdir (path_root, path_amp);
		[s2, m2, mid2] = mkdir (path_root, path_pos);
		prepdata (path_root, path_amp_AG, path_amp, sweeps);
		[s1, m1, mid1] = mkdir (path_root, path_amp_f);
		[s2, m2, mid2] = mkdir (path_root, path_pos_f);
		prepdata (path_root, path_amp_AG, path_amp_f, sweeps, ftype);
	else
		tapad (path_root, path_amp, path_pos, sweeps, sensors);  
		tapad (path_root, path_amp_f, path_pos_f, sweeps, sensors);  
	end
	% ----------------------------------------------------------------------- %
	sweeps	= 1:9;
	if (do_prep)
		path_root = '~/ContactData/Michele/Interference/sweeps_b1/';
		[s1, m1, mid1] = mkdir (path_root, path_amp);
		[s2, m2, mid2] = mkdir (path_root, path_pos);
		prepdata (path_root, path_amp_AG, path_amp, sweeps);
		[s1, m1, mid1] = mkdir (path_root, path_amp_f);
		[s2, m2, mid2] = mkdir (path_root, path_pos_f);
		prepdata (path_root, path_amp_AG, path_amp_f, sweeps, ftype);
	else
		tapad (path_root, path_amp, path_pos, sweeps, sensors);  
		tapad (path_root, path_amp_f, path_pos_f, sweeps, sensors);  
	end
	% ----------------------------------------------------------------------- %
end



	

	%prepdata (path_root, path_amp_AG, path_amp, sweeps);
%tapad    (path_root, path_amp,    path_pos, sweeps, sensors);  
