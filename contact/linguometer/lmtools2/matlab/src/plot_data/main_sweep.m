clear all;
% ---------------------------------------------------------------------------- %
% --- Load data (POS + AMP) --- 
root = 'cache';
kind = 'sweep';
%type = 'std';
type = 's';
exper = 3;
sweep = 4;

AG = lmProfile ();
[file_amp file_pos] = lmDataPath(root, exper, sweep, kind, type);
data = lmCollectData(AG, file_amp, file_pos);
% -----------------------------------------------------------------------------%
% --- Plot AMP and POS --- 
if (1)
	plot_pos (AG, data, sweep, 3000);
	plot_amp (AG, data, sweep, 4000);
end
% -----------------------------------------------------------------------------%
% --- Plot Cluster View --- 
	plot_pos_3dc(AG, data, sweep,  1:3, 1010);
	plot_pos_3dc(AG, data, sweep,  4:5, 1011);
	plot_pos_3dc(AG, data, sweep,  6:9, 1012);
	plot_pos_3dc(AG, data, sweep,  10:12, 1013);
% -----------------------------------------------------------------------------%
if (0)
	plot_pos_3dp
end
% ---------------------------------------------------------------------------- %
% --- ISD test --- 
if (0)
	ISD = plot_isd (AG, data, sweep, 11, 12, 1002);
	ISD = plot_isd (AG, data, sweep, 10, 11, 1003);
end

sweep_t 	= sprintf('%.4d', sweep);
exper_t 	= sprintf('%.4d', exper);

%mtExportAll(['temp/exp_' exper_t '/sweep_' sweep_t], 'png', 100);
%end

for s = 1:12
	printf('Sensor %2d\n', s);
	for ch = 1:6
		tstd = std(data.amp(:, ch, s)); 
		tavg = mean(data.amp(:, ch, s)); 
		printf('  %2d --> AVG = %.3f    STD = %.3f\n',ch, tavg, tstd);
	end
end
