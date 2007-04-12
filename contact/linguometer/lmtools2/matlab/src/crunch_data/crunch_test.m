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
