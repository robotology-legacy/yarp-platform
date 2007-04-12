clear all;
% ---------------------------------------------------------------------------- %
root_cache  = 'ContactCache';
root_data   = 'ContactData/Experiments';
file_amp_t 	= '~/RRRR/experiment_XXXX/AG/Sweeps/amps/SSSS.amp';
file_pos_t 	= '~/RRRR/experiment_XXXX/AG/Sweeps/lm_pos/SSSS.mat';
% ---------------------------------------------------------------------------- %
if (1)
	exper		= 12;
	sweep 		= 1;
	file_pos_t 	= '~/RRRR/experiment_XXXX/AG/Sweeps/m_rawpos/SSSS.mat';
	root_t		= root_cache;
end

if (0)
	exper		= 1;
	sweep 		= 1;
	root_t		= root_data;
end

if (0)
	exper		= 6;
	sweep 		= 1;
	root_t		= root_data;
end
% -----------------------------------------------------------------------------%
sweep_t 	= sprintf('%.4d', sweep);
exper_t 	= sprintf('%.4d', exper);

file_amp_t 	= regexprep (file_amp_t, 'RRRR', root_t);
file_amp_t 	= regexprep (file_amp_t, 'SSSS', sweep_t);
file_amp 	= regexprep (file_amp_t, 'XXXX', exper_t);

file_pos_t 	= regexprep (file_pos_t, 'RRRR', root_t);
file_pos_t 	= regexprep (file_pos_t, 'SSSS', sweep_t);
file_pos 	= regexprep (file_pos_t, 'XXXX', exper_t);
% -----------------------------------------------------------------------------%
AG = lmProfile ();
data_amp = lmLoadData (AG, file_amp);
data_pos = lmLoadData (AG, file_pos);

if (data_amp.samples == data_pos.samples)
	data = data_pos;
	data.amp = data_amp.amp;
	clear data_amp;
	clear data_pos;
end
% -----------------------------------------------------------------------------%
% --- Plot AMP and POS --- 
if (0)
	plot_pos (AG, data, sweep, 3000);
	plot_amp (AG, data, sweep, 4000);
	
	plot_pos_3dc(AG, data, sweep,  1, 1001);
end
% -----------------------------------------------------------------------------%
if (1)
	plot_pos_3dp
end

