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

function data = lmpkgAlign2(seq, num, opt_plot, opt_invert, opt_bug);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Options                         %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
opt_run_as_script 	= 1;
opt_plot_filter 	= 0;

if (opt_run_as_script)
	seq = 0;
	num = 1;
	opt_plot = 1;
	opt_invert = 1;
	opt_bug = 1;
end

% LG data is not always available.
% If the LG data is not found, it will
% not be used.
opt_nolg = 0;

% Target sampling rate for signal alignment
rate_std = 48000;

% Let's create the data structure. Giampiero Salvi from IST
% said that it would be a good idea to keep the original data
% inside the MAT file.
data = {};


% Filters used for smoothing the signals
filter_fast = ones(round(rate_std*0.01),1);
filter_slow = ones(round(rate_std*0.1),1);
if (opt_plot_filter)
	mtPlotFFT(filter_fast, 48e3, 10);
end

% Cook names for the audio files (reference)
audio_ag = sprintf('seq_%.4d/wd_%.4d_ag.wav', seq, num);
audio_cc = sprintf('seq_%.4d/wd_%.4d_cc.wav', seq, num);
audio_us = sprintf('seq_%.4d/wd_%.4d_us.wav', seq, num);
audio_lg = sprintf('seq_%.4d/wd_%.4d_lg.wav', seq, num);
% ------------------- %
file_agamp = sprintf('seq_%.4d/wd_%.4d_ag.amp', seq, num); 
file_agpos = sprintf('seq_%.4d/wd_%.4d_ag.pos', seq, num); 
file_usff = sprintf('seq_%.4d/wd_%.4d_us.ff', seq, num); 
file_ccff = sprintf('seq_%.4d/wd_%.4d_cc.ff', seq, num); 


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Load data                       %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Load US, AG and CC data
printf('[lmpkgAlign2] Loading data\n');
try
	[owav_ag  orate_ag] = wavread(audio_ag);
	[owav_cc  orate_cc] = wavread(audio_cc);
	[owav_us  orate_us] = wavread(audio_us);
	% ------------------- %
	odat_agamp = importdata(file_agamp);
	odat_agpos = importdata(file_agpos);
	odat_usff  = importdata(file_usff);
	odat_ccff  = importdata(file_ccff);
catch
	printf('[lmpkgAlign2] Something very bad happened while loading data...\n');
	return 
end
	
% Load LG signals
try
	[owav_lg  orate_lg]	= wavread(audio_lg);
catch
	printf('[lmpkgAlign2] LG signal not available...\n');
	opt_nolg = 1;
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Perform some useful corrections %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if (opt_invert)
	printf('[lmpkgAlign2] Inverting signals\n');
	owav_us = -owav_us;
	owav_ag = -owav_ag;
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Save RAW data                   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
printf('[lmpkgAlign2] Saving RAW data\n');
data.raw.CC.spc 		= owav_cc;
data.raw.CC.spc_rate 	= orate_cc;
data.raw.US.spc 		= owav_us;
data.raw.US.spc_rate 	= orate_us;
data.raw.AG.spc 		= owav_ag;
data.raw.AG.spc_rate 	= orate_ag;
if (opt_nolg == 0)
	data.raw.LG.spc 		= owav_lg(:, 1);
	data.raw.LG.spc_rate 	= orate_lg;
	data.raw.LG.egg 		= owav_lg(:, 2);
	data.raw.LG.egg_rate 	= orate_lg;
end
% ------------------- %
data.raw.AG.amp 		= odat_agamp;
data.raw.AG.amp_rate 	= 200;
data.raw.AG.pos 		= odat_agpos;
data.raw.AG.pos_rate 	= 200;
% ------------------- %
data.raw.US.fea 		= odat_usff;
data.raw.US.fea_rate 	= 25;
data.raw.CC.fea 		= odat_ccff;
data.raw.CC.fea_rate 	= 25;
% ------------------- %


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Resample the dataset            %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
printf('[lmpkgAlign2] Resampling RAW data\n');
data.pre.CC.spc 		= data.raw.CC.spc(:, 1);
data.pre.CC.spc_rate 	= data.raw.CC.spc_rate;
data.pre.US.spc 		= data.raw.US.spc(:, 2);
data.pre.US.spc_rate 	= data.raw.US.spc_rate;
data.pre.AG.spc 		= resample(data.raw.AG.spc, rate_std, data.raw.AG.spc_rate);
data.pre.AG.spc_rate 	= rate_std;
if (opt_nolg == 0)
	data.pre.LG.spc 		= resample(data.raw.LG.spc, rate_std, data.raw.LG.spc_rate);
	data.pre.LG.spc_rate 	= rate_std;
	data.pre.LG.egg 		= resample(data.raw.LG.egg, rate_std, data.raw.LG.egg_rate);
	data.pre.LG.egg_rate 	= rate_std;
end
% ------------------- %
data.pre.AG.amp 		= resample(data.raw.AG.amp, rate_std, data.raw.AG.amp_rate, 0);
data.pre.AG.amp_rate	= rate_std;
data.pre.AG.pos 		= resample(data.raw.AG.pos, rate_std, data.raw.AG.pos_rate, 0);
data.pre.AG.pos_rate	= rate_std;
% ------------------- %
%data.pre.US.fea 		= resample(data.raw.US.fea, rate_std, data.raw.US.fea_rate, 0);
data.pre.US.fea			= lmpkgResample(data.raw.US.fea, rate_std/data.raw.US.fea_rate);
data.pre.US.fea_rate 	= rate_std;
%data.pre.CC.fea 		= resample(data.raw.CC.fea, rate_std, data.raw.CC.fea_rate, 0);;
data.pre.CC.fea			= lmpkgResample(data.raw.CC.fea, rate_std/data.raw.CC.fea_rate);
data.pre.CC.fea_rate 	= rate_std;
% ------------------- %


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Zap clicks                      %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
printf('[lmpkgAlign2] Zapping clicks\n');
[data.pre.US.spc zap0_us zap1_us] = lmpkgZap(data.pre.US.spc, rate_std, opt_bug);
data.pre.US.spc_rate 	= rate_std;
[data.pre.AG.spc zap0_ag zap1_ag] = lmpkgZap(data.pre.AG.spc, rate_std, opt_bug);
data.pre.AG.spc_rate 	= rate_std;

if (zap1_ag < zap0_ag)
	while (1)
		printf('[lmpkgAlign2] AG zap problem\n');
		pause(60);
	end
end
if (zap1_us < zap0_us) 
	while (1)
		printf('[lmpkgAlign2] US zap problem\n');
		pause(60);
	end
end
std_zap0 = max([zap0_us zap0_ag]);
std_zap1 = min([zap1_us zap1_ag]);



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Bootstrapping alignement        %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
data.nrg1 = lmpkgComputeEnergy (data.pre, filter_fast, 'second pass', opt_nolg);
data.off1 = lmpkgComputeOffset (data.nrg1, 'first pass', opt_nolg);
lmpkgPrintLenghts (data.pre, 'PRE', opt_nolg)
lmpkgPrintOffsets (data.off1, 'OFF1', opt_nolg)


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Toward fine aln: align          %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
data.lag = data.pre;

printf('[lmpkgAlign2] Performing fine alignement\n');
printf('  AG.spc\n');
data.lag.AG.spc = lagmatrix(data.pre.AG.spc, data.off1.ag);
printf('  AG.amp\n');
data.lag.AG.amp = lagmatrix(data.pre.AG.amp, data.off1.ag);
printf('  AG.pos\n');
data.lag.AG.pos = lagmatrix(data.pre.AG.pos, data.off1.ag);

if (opt_nolg == 0)
	printf('  LG.spc\n');
	data.lag.LG.spc = lagmatrix(data.pre.LG.spc, data.off1.lg);
	printf('  LG.egg\n');
	data.lag.LG.egg = lagmatrix(data.pre.LG.egg, data.off1.lg);
end

printf('  CC.spc\n');
data.lag.CC.spc = lagmatrix(data.pre.CC.spc, data.off1.cc);
printf('  CC.fea    ');
for col = 1:min(size(data.pre.CC.fea))
	printf('%d/%d ', col, min(size(data.pre.CC.fea)));
	data.lag.CC.fea(:, col) = lagmatrix(data.pre.CC.fea(:, col), data.off1.cc);
end
printf('\n');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Toward fine aln: crop NaN       %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
data.aln = data.lag;
data.aln.AG.spc = lmpkgCrop(data.lag.AG.spc, data.off1.ag);
data.aln.AG.amp = lmpkgCrop(data.lag.AG.amp, data.off1.ag);
data.aln.AG.pos = lmpkgCrop(data.lag.AG.pos, data.off1.ag);
data.aln.CC.spc = lmpkgCrop(data.lag.CC.spc, data.off1.cc);
data.aln.CC.fea = lmpkgCrop(data.lag.CC.fea, data.off1.cc);
if (opt_nolg == 0)
	data.aln.LG.spc = lmpkgCrop(data.lag.LG.spc, data.off1.lg);
	data.aln.LG.egg = lmpkgCrop(data.lag.LG.egg, data.off1.lg);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Toward fine aln: crop samples   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
lengths = zeros(4, 1);
lengths(1) = length(data.aln.US.spc);
lengths(2) = length(data.aln.AG.spc);
lengths(3) = length(data.aln.CC.spc);
if (opt_nolg == 0)
	lengths(4) = length(data.aln.LG.spc);
else
	lengths(4) = 1e6;
end

if (std_zap1 <= min(lengths))
	std_lenght = std_zap1;
else
	std_lenght = min(lengths);
end

data.misc.zap.s0 = std_zap0;
data.misc.zap.s1 = std_lenght;

data.aln.US.spc = data.aln.US.spc(std_zap0:std_lenght);
data.aln.US.fea = data.aln.US.fea(std_zap0:std_lenght, :);
data.aln.CC.spc = data.aln.CC.spc(std_zap0:std_lenght);
data.aln.CC.fea = data.aln.CC.fea(std_zap0:std_lenght, :);
data.aln.AG.spc = data.aln.AG.spc(std_zap0:std_lenght);
data.aln.AG.amp = data.aln.AG.amp(std_zap0:std_lenght, :);
data.aln.AG.pos = data.aln.AG.pos(std_zap0:std_lenght, :);
if (opt_nolg == 0)
	data.aln.LG.spc = data.aln.LG.spc(std_zap0:std_lenght);
	data.aln.LG.egg = data.aln.LG.egg(std_zap0:std_lenght);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Computing offset                %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
data.nrg2 = lmpkgComputeEnergy (data.aln, filter_fast, 'second pass', opt_nolg);
data.off2 = lmpkgComputeOffset (data.nrg2, 'second pass', opt_nolg);
lmpkgPrintLenghts (data.aln, 'ALN', opt_nolg)
lmpkgPrintOffsets (data.off2, 'OFF2', opt_nolg)


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Write description entries       %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
lmpkgWriteDescription (data)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Build few structures            %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
data.misc.status.CC = 1;
data.misc.status.AG = 1;
data.misc.status.US = 1;
data.misc.status.LG = opt_nolg;
data.misc.time = [0:1/rate_std:(length(data.aln.US.spc) - 1)/rate_std];
data.misc.rate = rate_std;
data.misc.length = length(data.aln.US.spc);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Save an image. Debug friendly   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
lmpkgPlotAlignment (3, data.aln, data.nrg2, rate_std, opt_nolg);
file_img = sprintf('log/plots/lmpack_%.4d_%.4d', seq, num);
mtExport(3, file_img, '', 'png', 80);






function lmpkgPrintLenghts (dataptr, type, opt_nolg)
	printf('[lmpkgPrintLenghts] Lenght of %s signals\n', type);
	printf('  US.spc %d\n', length(dataptr.US.spc));
	printf('  CC.spc %d\n', length(dataptr.CC.spc));
	printf('  AG.spc %d\n', length(dataptr.AG.spc));
	if (opt_nolg == 0)
		printf('  LG.spc %d\n', length(dataptr.LG.spc));
	end
	printf('     ---\n');
	printf('  US.fea %d\n', length(dataptr.US.fea));
	printf('  CC.fea %d\n', length(dataptr.CC.fea));
	printf('  AG.amp %d\n', length(dataptr.AG.amp));
	printf('  AG.pos %d\n', length(dataptr.AG.pos));
	if (opt_nolg == 0)
		printf('  LG.egg %d\n', length(dataptr.LG.egg));
	end

function lmpkgPrintOffsets (offptr, type, opt_nolg)
	printf('[lmpkgPrintOffsets] %s offset values:\n', type);
	printf('  us    %d (Reference)\n', offptr.us);
	printf('  cc    %d\n', offptr.cc);
	printf('  ag    %d\n', offptr.ag);
	if (opt_nolg == 0)
		printf('  lg    %d\n', offptr.lg);
	end

function offptr = lmpkgComputeOffset (nrgptr, type, opt_nolg)
	printf('[lmpkgComputeOffset] Computing offset (%s):\n', type);
	offptr.us = 0;
	offptr.ag = lmpkgLag(nrgptr.us, nrgptr.ag);
	offptr.cc = lmpkgLag(nrgptr.us, nrgptr.cc);
	if (opt_nolg == 0)
		offptr.lg = lmpkgLag(nrgptr.us, nrgptr.lg);
	end

function nrgptr = lmpkgComputeEnergy (alnptr, filter, type, opt_nolg)
	printf('[lmpkgComputeEnergy] Creating alignment energy-like signals, %s\n', type);
	nrgptr.us = filter2(filter, abs(alnptr.US.spc), 'same');
	nrgptr.ag = filter2(filter, abs(alnptr.AG.spc), 'same');
	nrgptr.cc = filter2(filter, abs(alnptr.CC.spc), 'same');
	if (opt_nolg == 0)
		nrgptr.lg = filter2(filter, abs(alnptr.LG.spc), 'same');
	end

function lmpkgWriteDescription (dataptr)
	dataptr.raw.description   = '[step-1] RAW  Data: signals loaded from SEQ folders';
	dataptr.pre.description   = '[step-2] PRE  Data: resampled RAW Data';
	dataptr.nrg1.description  = '[step-3] NRG1 Data: energy-like signals from PRE Data';
	dataptr.off1.description  = '[step-4] OFF1 Data: PRE Data after initial offset correction';
	dataptr.lag.description   = '[step-5] LAG  Data: OFF Data before offset correction';
	dataptr.aln.description   = '[step-5] ALN  Data: LAG Data afterptr fine alignement';
	dataptr.nrg2.description  = '[step-6] NRG2 Data: energy-like signals from ALN Data';
	dataptr.off2.description  = '[step-7] OFF2 Data: OFF Data after offset correction';

function lmpkgPlotAlignment (fig, alnptr, nrgptr, rate_std, opt_nolg)
	time = [0:1/rate_std:(length(alnptr.US.spc) - 1)/rate_std];
	mtSimpleFig(3);

	subplot(4, 3, 1);
	plot(time, alnptr.US.spc, 'r');
	grid on;
	axis tight;
	ylabel('US-Speech');
	title(sprintf('Alignment reference tracks:\nspeech'));
	
	subplot(4, 3, 4);
	plot(time, alnptr.AG.spc, 'b');
	grid on;
	axis tight;
	ylabel('AG-Speech');
	
	if (opt_nolg == 0)
		subplot(4, 3, 7);
		plot(time, alnptr.LG.spc, 'k');
		grid on;
		axis tight;
		ylabel('LG-Speech');
	end

	subplot(4, 3, 10);
	plot(time, alnptr.CC.spc, 'g');
	grid on;
	axis tight;
	ylabel('CC-Speech');
	xlabel('Time [s]');
		
	subplot(4, 3, 2);
	plot(time, nrgptr.us, 'r');
	grid on;
	axis tight;
	title(sprintf('Alignment reference tracks:\nenergy-like'));
	
	subplot(4, 3, 5);
	plot(time, nrgptr.ag, 'b');
	grid on;
	axis tight;
	
	if (opt_nolg == 0)
		subplot(4, 3, 8);
		plot(time, nrgptr.lg, 'k');
		grid on;
		axis tight;
	end
	
	subplot(4, 3, 11);
	plot(time,  nrgptr.cc, 'g');
	grid on;
	axis tight;
	xlabel('Time [s]');

	if (opt_nolg == 0)
		subplot(4, 3, 9);
		plot(time, alnptr.LG.egg, 'k');
		grid on;
		axis tight;
		ylabel('LG-Data');
		xlabel('Time [s]');
	end
	drawnow;
