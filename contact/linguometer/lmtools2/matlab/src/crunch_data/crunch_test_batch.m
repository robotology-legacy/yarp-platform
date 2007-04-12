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
