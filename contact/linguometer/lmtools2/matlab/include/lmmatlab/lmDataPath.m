function [file_amp file_pos] = lmDataPath (root, exper, sweep, kind, type)
% Usage:
%   [amp pos] = lmDataPath (root, exper, sweep, kind, type)
% Examples:
%   lmDataPath('cache', 1, 2, 'sweep', 'std')
%   lmDataPath('root', 1, 2, 'sweep', 'fil')
%   lmDataPath('/mnt/data', 1, 2, 'test, 'ag')

if (strcmp(root, 'cache'))
	root = 'ContactCache';
elseif (strcmp(root, 'data'))
	root = 'ContactData/Experiments';
end

switch(kind)
	case {'s' 'Sweep' 'sweep' 'sweeps'}
		kind = 'Sweeps';
	case {'t' 'Test' 'test' 'tests'}
		kind = 'Test';
end

switch(type)
	case {'std' 's'}
		type_amp = 'lm_amps';
		type_pos = 'lm_pos';
		ext_amp  = 'mat';
		ext_pos  = 'mat';
	case {'fil' 'f'}
		type_amp = 'lm_amps_f';
		type_pos = 'lm_pos_f';
		ext_amp  = 'mat';
		ext_pos  = 'mat';
	case {'ag' ''}
		type_amp = 'amps';
		type_pos = 'pos';
		ext_amp  = 'amp';
		ext_pos  = 'pos';
	otherwise
		%handle error
end

file_amp_t 	= '~/RRRR/experiment_XXXX/AG/KKKK/TTTT/SSSS.EEE';
file_pos_t 	= '~/RRRR/experiment_XXXX/AG/KKKK/TTTT/SSSS.EEE';

sweep_t 	= sprintf('%.4d', sweep);
exper_t 	= sprintf('%.4d', exper);

file_amp_t 	= regexprep (file_amp_t, 'RRRR', root);
file_amp_t 	= regexprep (file_amp_t, 'SSSS', sweep_t);
file_amp_t 	= regexprep (file_amp_t, 'XXXX', exper_t);
file_amp_t 	= regexprep (file_amp_t, 'TTTT', type_amp);
file_amp_t 	= regexprep (file_amp_t, 'KKKK', kind);
file_amp 	= regexprep (file_amp_t, 'EEE',  ext_amp);

file_pos_t 	= regexprep (file_pos_t, 'RRRR', root);
file_pos_t 	= regexprep (file_pos_t, 'SSSS', sweep_t);
file_pos_t 	= regexprep (file_pos_t, 'XXXX', exper_t);
file_pos_t 	= regexprep (file_pos_t, 'TTTT', type_pos);
file_pos_t 	= regexprep (file_pos_t, 'KKKK', kind);
file_pos 	= regexprep (file_pos_t, 'EEE',  ext_pos);
