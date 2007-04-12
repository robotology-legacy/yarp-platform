clear all;
% ---------------------------------------------------------------------------- %
root_cache  = 'ContactCache';
root_data   = 'ContactData/Experiments';
file_amp_t 	= '~/RRRR/experiment_XXXX/AG/Test/amps/SSSS.amp';
file_pos_t 	= '~/RRRR/experiment_XXXX/AG/Test/lm_pos/SSSS.mat';
% ---------------------------------------------------------------------------- %
if (0)
	exper		= 3;
	sweep 		= 1;
	root_t		= root_cache;
end

if (0)
	exper		= 2;
	sweep 		= 1;
	root_t		= root_data;
end

if (1) % :-(
	exper		= 2;
	sweep 		= 2;
	root_t		= root_cache;
end
% ---------------------------------------------------------------------------- %
sweep_t 	= sprintf('%.4d', sweep);
exper_t 	= sprintf('%.4d', exper);

file_amp_t 	= regexprep (file_amp_t, 'RRRR', root_t);
file_amp_t 	= regexprep (file_amp_t, 'SSSS', sweep_t);
file_amp 	= regexprep (file_amp_t, 'XXXX', exper_t);

file_pos_t 	= regexprep (file_pos_t, 'RRRR', root_t);
file_pos_t 	= regexprep (file_pos_t, 'SSSS', sweep_t);
file_pos 	= regexprep (file_pos_t, 'XXXX', exper_t);
% ---------------------------------------------------------------------------- %
AG = lmProfile ();
data_amp = lmLoadData (AG, file_amp);
data_pos = lmLoadData (AG, file_pos);

if (data_amp.samples == data_pos.samples)
	data = data_pos;
	data.amp = data_amp.amp;
	clear data_amp;
	clear data_pos;
end

% ---------------------------------------------------------------------------- %
% --- Plot AMP and POS --- 
if (1)
	plot_amp (AG, data, sweep, 2000);
	plot_pos (AG, data, sweep, 3000);
	
	plot_pos_3dc (AG, data, sweep,  1:12, 1001)
end
% ---------------------------------------------------------------------------- %
% --- Still analysis --- 
if (0)
	data_still = lmTrimDataT(AG, data, 220, 230);
	plot_amp(AG, data_still, sweep, 3000);
	plot_pos(AG, data_still, sweep, 3500);

	%	data_fstill = lmFilterData(data_still, 0);
	%	plot_amp (AG, data_fstill, sweep, 4000);

	plot_pos_3dc(AG, data_still, sweep,  1, 1001)
	plot_pos_3dc(AG, data_still, sweep, 12, 1002)
end
% ---------------------------------------------------------------------------- %
% --- US analysis --- 
if (0)
	data_US = lmTrimDataT(AG, data, 70, 100);
	plot_amp(AG, data_US, sweep, 3000);
	plot_pos(AG, data_US, sweep, 3500);

	plot_pos_3dc(AG, data_US, sweep,  1, 1001)
	plot_pos_3dc(AG, data_US, sweep, 12, 1002)
	
	amp_US_01 = data_US.amp (:, :, 1);
	amp_US_12 = data_US.amp (:, :, 12);
	mtPlotFFT(amp_US_01(:, 1)', AG.pars.sr, 1003);
	mtPlotFFT(amp_US_12(:, 1)', AG.pars.sr, 1003);
end
% ---------------------------------------------------------------------------- %
% --- Downsampling test --- 
if (0)
	sr_f = 10;		% New sampling rate = AG.pars.sr / sr_f (200/8 = 25)

	for sensor = 1:12
		for coil = 1:6
			data_DC.amp(:, coil, sensor) = decimate(data.amp(:, coil, sensor), sr_f);
		end
	end
	
	for sensor = 1:12
		for dimension = 1:6
			data_DC.pos(:, dimension, sensor) = decimate(data.pos(:, dimension, sensor), sr_f);
		end
	end
end
% ---------------------------------------------------------------------------- %
% --- ISD test --- 
if (0)
	for sample = 1:data.samples
		pA = data.pos(sample, 1:3, 2);
		pB = data.pos(sample, 1:3, 3);
		ISD(sample) = mtED (pA, pB);
	end
end

