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
