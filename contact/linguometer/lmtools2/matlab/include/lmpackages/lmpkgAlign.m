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

%function data = lmpkgAlign(seq, num, opt_plot, opt_invert, opt_bug);
if (1)
	seq = 0;
	num = 1;
	opt_plot = 1;
	opt_invert = 1;
	opt_bug = 1;
end
opt_nolg = 0;
data = {};
printf('[lmpkgAlign] Running on SEQ %d, word %d\n', seq, num);

% Few options, mainly for debug/devel
std_rate = 48000;
opt_plotcrap = 0;
%opt_invert = 0;
opt_spam = 0;

% Let's create the data structure. Giampiero Salvi from IST
% said that it would be a good idea to keep the original data
% inside the MAT file.
data = {};

% Filters used for smoothing the signals
filter_fast = ones(round(std_rate*0.01),1);
filter_slow = ones(round(std_rate*0.1),1);
if (0)
	mtPlotFFT(filter_fast, 48e3, 10);
end


% Load audio files (reference)
printf('[lmpkgAlign] Loading data\n');
audio_wd = sprintf('seq_%.4d/wd_%.4d_us.wav', seq, num);
audio_ag = sprintf('seq_%.4d/wd_%.4d_ag.wav', seq, num);
audio_lg = sprintf('seq_%.4d/wd_%.4d_lg.wav', seq, num);
audio_cc = sprintf('seq_%.4d/wd_%.4d_cc.wav', seq, num);
[owav_wd  orate_wd]  = wavread(audio_wd);
[owav_ag  orate_ag]  = wavread(audio_ag);
[owav_cc  orate_cc]  = wavread(audio_cc);
try
	[owav_lg  orate_lg]  = wavread(audio_lg);
	data.raw.LG.spc = owav_lg(:, 1);
	data.raw.LG.spc_rate = 16000;
	data.raw.LG.egg = owav_lg(:, 2);
	data.raw.LG.egg_rate = 16000;
	opt_nolg = 0;
catch
	opt_nolg = 1;
end

% Select the right channel (US-Speech)
printf('[lmpkgAlign] Zapping US data\n');
wav_wd = owav_wd(:, 2);
if (opt_invert)
	wav_wd = -wav_wd;
end
% Save RAW data
data.raw.US.spc = wav_wd;
data.raw.US.spc_rate = 48000;
[wav_wd zap0_wd zap1_wd] = lmpkgZap(wav_wd, std_rate, opt_bug);


% Resample the AG-Speech data (16-->48kHz)
printf('[lmpkgAlign] Zapping AG data\n');
wav_ag = resample(owav_ag, std_rate, orate_ag);
if (opt_invert)
	wav_ag = -wav_ag;
end

% Save RAW data
data.raw.AG.spc = wav_ag;
data.raw.AG.spc_rate = 16000;

rate_ag = std_rate;
[wav_ag zap0_ag zap1_ag] = lmpkgZap(wav_ag, std_rate, opt_bug);

if (zap1_ag < zap0_ag)
	printf('[lmpkgAlign] AG zap problem\n');
    return;
end

if (zap1_wd < zap0_wd) 
	printf('[lmpkgAlign] US zap problem\n');
    return;
end

% Select the left channel (LG-Speech)
printf('[lmpkgAlign] Processing remaining data\n');
if (opt_nolg == 0)
	wav_lg = resample(owav_lg, std_rate, orate_lg);
	dat_lg = wav_lg(:, 2);
	wav_lg = wav_lg(:, 1);
end


% Save RAW data
data.raw.CC.spc = owav_cc;
data.raw.CC.spc_rate = 48000;
% Select the left CC-Speech channel
wav_cc = owav_cc(:, 1);

std_zap0 = max([zap0_wd zap0_ag]);
std_zap1 = min([zap1_wd zap1_ag]);

printf('[lmpkgAlign] Loading non-audio data\n');
% Load AGAMP
file_agamp = sprintf('seq_%.4d/wd_%.4d_ag.amp', seq, num); 
odat_agamp = importdata(file_agamp);
data.raw.AG.amp = odat_agamp;
data.raw.AG.amp_rate = 200;
dat_agamp = resample(odat_agamp, std_rate, 200, 0);
% Load AGPOS
file_agpos = sprintf('seq_%.4d/wd_%.4d_ag.pos', seq, num); 
odat_agpos = importdata(file_agpos);
data.raw.AG.pos = odat_agpos;
data.raw.AG.pos_rate = 200;
dat_agpos = resample(odat_agpos, std_rate, 200, 0);
% Load USFF
file_usff = sprintf('seq_%.4d/wd_%.4d_us.ff', seq, num); 
odat_usff = importdata(file_usff);
data.raw.US.fea = odat_usff;
data.raw.US.fea_rate = 25;
%%%%%%%%odat_usff = odat_usff(:,2:min(size(odat_usff)));
odat_usff = odat_usff(:,1:min(size(odat_usff)));
dat_usff = resample(odat_usff, std_rate, 25, 0);
% Load CCFF
file_ccff = sprintf('seq_%.4d/wd_%.4d_cc.ff', seq, num); 
odat_ccff = importdata(file_ccff);
data.raw.CC.fea = odat_ccff;
data.raw.CC.fea_rate = 25;
%%%%%%%%odat_ccff = odat_ccff(:,2:min(size(odat_ccff)));
odat_ccff = odat_ccff(:,1:min(size(odat_ccff)));
dat_ccff = resample(odat_ccff, std_rate, 25, 0);

printf('[lmpkgAlign] Creating alignment energy-like signals\n');
sig_wd  = filter2(filter_fast, abs(wav_wd), 'same');
sig_ag  = filter2(filter_fast, abs(wav_ag), 'same');
if (opt_nolg == 0)
	sig_lg  = filter2(filter_fast, abs(wav_lg), 'same');
end
sig_cc  = filter2(filter_fast, abs(wav_cc), 'same');
sig_cc  = filter2(filter_fast, abs(wav_cc), 'same');

printf('[lmpkgAlign] Initial offset values:\n');
offset_ag = lmpkgLag(sig_wd, sig_ag);
if (opt_nolg == 0)
	offset_lg = lmpkgLag(sig_wd, sig_lg);
end
offset_cc = lmpkgLag(sig_wd, sig_cc);
printf('  WD     %d (reference)\n', 0);
printf('  AG     %d\n', offset_ag);
if (opt_nolg == 0)
	printf('  LG     %d\n', offset_lg);
end
printf('  CC     %d\n', offset_cc);

printf('[lmpkgAlign] Initial lenght of signals\n');
printf('  WD     %d\n', length(wav_wd));
printf('  AG     %d\n', length(wav_ag));
printf('  AGAMP  %d\n', length(dat_agamp));
printf('  AGPOS  %d\n', length(dat_agpos));
if (opt_nolg == 0)
	printf('  LG     %d\n', length(wav_lg));
end
printf('  CC     %d\n', length(wav_cc));

printf('[lmpkgAlign] Correcting AG lag\n');
wav2_ag = lagmatrix(wav_ag, offset_ag);

printf('[lmpkgAlign] Correcting AG-AMP lag\n');
dat2_agamp = lagmatrix(dat_agamp, offset_ag);
%for col = 1:min(size(dat_agamp))
%	printf('%d/%d ', col, min(size(dat_agamp)));
%	dat2_agamp(:, col) = lagmatrix(dat_agamp(:, col), offset_cc);
%end
%printf('\n');

printf('[lmpkgAlign] Correcting AG-POS lag\n');
dat2_agpos = lagmatrix(dat_agpos, offset_ag);
%for col = 1:min(size(dat_agpos))
%	printf('%d/%d ', col, min(size(dat_agpos)));
%	dat2_agpos(:, col) = lagmatrix(dat_agpos(:, col), offset_cc);
%end
%printf('\n');

if (opt_nolg == 0)
	printf('[lmpkgAlign] Correcting LG-Speech lag\n');
	wav2_lg = lagmatrix(wav_lg, offset_lg);

	printf('[lmpkgAlign] Correcting LG-EEG lag\n');
	dat2_lg = lagmatrix(dat_lg, offset_lg);
end

printf('[lmpkgAlign] Correcting CC-Speech lag\n');
wav2_cc = lagmatrix(wav_cc, offset_cc);

printf('[lmpkgAlign] Correcting CC-Features lag: ');
% In the case of big delays, lagmatrix takes ages to finish
%dat2_ccff = lagmatrix(dat_ccff, offset_cc);
for col = 1:min(size(dat_ccff))
	printf('%d/%d ', col, min(size(dat_ccff)));
	dat2_ccff(:, col) = lagmatrix(dat_ccff(:, col), offset_cc);
end
printf('\n');

if (opt_plot && opt_plotcrap)
	mtSimpleFig(1);
	plot(sig_wd, 'r');
	hold on;
	plot(sig_ag, 'b');
	if (opt_nolg == 0)
		plot(sig_lg, 'k');
	end
	plot(sig_cc, 'g');
	hold off;
	grid on;
	axis tight;
	xlabel('samples');
	ylabel('energy');
	title('LM Fine Alignment: before');
	legend('WD', sprintf('AG (%d)', offset_ag), sprintf('LG (%d)', offset_lg), sprintf('CC  (%d)', offset_cc));
end


% remove NaN values
wav3_wd = wav_wd;
wav3_ag = lmpkgCrop(wav2_ag, offset_ag);
dat3_agamp = lmpkgCrop(dat2_agamp, offset_ag);
dat3_agpos = lmpkgCrop(dat2_agpos, offset_ag);
if (opt_nolg == 0)
	wav3_lg = lmpkgCrop(wav2_lg, offset_lg);
	dat3_lg = lmpkgCrop(dat2_lg, offset_lg);
end
wav3_cc = lmpkgCrop(wav2_cc, offset_cc);
dat3_ccff = lmpkgCrop(dat2_ccff, offset_cc);
dat3_usff = dat_usff;

lengths = zeros(4, 1);
lengths(1) = length(wav3_wd);
lengths(2) = length(wav3_ag);
if (opt_nolg == 0)
	lengths(3) = length(wav3_lg);
else
	lengths(3) = 1e6;
end
lengths(4) = length(wav3_cc);

if (std_zap1 <= min(lengths))
	std_lenght = std_zap1;
else
	std_lenght = min(lengths);
end

wav3_wd = wav3_wd(std_zap0:std_lenght);
wav3_ag = wav3_ag(std_zap0:std_lenght);
dat3_agamp = dat3_agamp(std_zap0:std_lenght, :);
dat3_agpos = dat3_agpos(std_zap0:std_lenght, :);
if (opt_nolg == 0)
	wav3_lg = wav3_lg(std_zap0:std_lenght);
	dat3_lg = dat3_lg(std_zap0:std_lenght);
end
wav3_cc = wav3_cc(std_zap0:std_lenght);
dat3_usff = dat3_usff(std_zap0:std_lenght, :);
dat3_ccff = dat3_ccff(std_zap0:std_lenght, :);

sig3_wd = filter2(filter_fast, abs(wav3_wd), 'same');
sig3_ag = filter2(filter_fast, abs(wav3_ag), 'same');
if (opt_nolg == 0)
	sig3_lg = filter2(filter_fast, abs(wav3_lg), 'same');
end
sig3_cc = filter2(filter_fast, abs(wav3_cc), 'same');

printf('[lmpkgAlign] Final offset values:\n');
offset3_ag = lmpkgLag(sig3_wd, sig3_ag);
if (opt_nolg == 0)
	offset3_lg = lmpkgLag(sig3_wd, sig3_lg);
end
offset3_cc = lmpkgLag(sig3_wd, sig3_cc);
printf('  WD   %d (reference)\n', 0);
printf('  AG   %d\n', offset3_ag);
if (opt_nolg == 0)
	printf('  LG   %d\n', offset3_lg);
end
printf('  CC   %d\n', offset3_cc);


if (opt_plot && opt_plotcrap)
	mtSimpleFig(2);
	plot(sig3_wd, 'r');
	hold on;
	plot(sig3_ag, 'b');
	if (opt_nolg == 0)
		plot(sig3_lg, 'k');
	end
	plot(sig3_cc, 'g');
	hold off;
	grid on;
	axis tight;
	xlabel('samples');
	ylabel('energy');
	title('LM Fine Alignment: before');
	legend('WD', 'AG', 'LG', 'CC ');
end

if (opt_plot)
	time = [0:1/std_rate:(length(wav3_wd) - 1)/std_rate];
	mtSimpleFig(3);

	subplot(4, 3, 1);
	plot(time, wav3_wd, 'r');
	grid on;
	axis tight;
	ylabel('US-Speech');
	title(sprintf('Alignment reference tracks:\nspeech'));
	
	subplot(4, 3, 4);
	plot(time, wav3_ag, 'b');
	grid on;
	axis tight;
	ylabel('AG-Speech');
	
	if (opt_nolg == 0)
		subplot(4, 3, 7);
		plot(time, wav3_lg, 'k');
		grid on;
		axis tight;
		ylabel('LG-Speech');
	end

	subplot(4, 3, 10);
	plot(time, wav3_cc, 'g');
	grid on;
	axis tight;
	ylabel('CC-Speech');
	xlabel('Time [s]');
		
	subplot(4, 3, 2);
	plot(time, filter2(filter_fast, abs(wav3_wd), 'same'), 'r');
	grid on;
	axis tight;
	title(sprintf('Alignment reference tracks:\nenergy-like'));
	
	subplot(4, 3, 5);
	plot(time, filter2(filter_fast, abs(wav3_ag), 'same'), 'b');
	grid on;
	axis tight;
	
	if (opt_nolg == 0)
		subplot(4, 3, 8);
		plot(time, filter2(filter_fast, abs(wav3_lg), 'same'), 'k');
		grid on;
		axis tight;
	end
	
	subplot(4, 3, 11);
	plot(time, filter2(filter_fast, abs(wav3_cc), 'same'), 'g');
	grid on;
	axis tight;
	xlabel('Time [s]');

	if (opt_plotcrap)
		subplot(4, 3, 3);
		mesh(resample(dat3_agamp, 25, std_rate));
		grid on;
		axis tight;
		ylabel('Samples');
		xlabel('Current');
		zlabel('AG-Amp');
		title('Data tracks');
	end

	if (opt_nolg == 0)
		subplot(4, 3, 9);
		plot(time, dat3_lg, 'k');
		grid on;
		axis tight;
		ylabel('LG-Data');
		xlabel('Time [s]');
	end

	drawnow;
	file_img = sprintf('log/plots/lmpack_%.4d_%.4d', seq, num);
	mtExport(3, file_img, '', 'png', 80);
end

printf('[lmpkgAlign] Final lenght of signals\n');
printf('  (+) US-Speech   %d\n', length(wav3_wd));
printf('  (+) US-Features %d\n', length(dat3_usff));
printf('  (+) AG-Speech   %d\n', length(wav3_ag));
printf('  (+) AG-AMP      %d\n', length(dat3_agamp));
printf('  (+) AG-POS      %d\n', length(dat3_agpos));
if (opt_nolg == 0)
	printf('  (+) LG-Speech   %d\n', length(wav3_lg));
	printf('  (+) LG-EEG      %d\n', length(dat3_lg));
end
printf('  (+) CC-Speech   %d\n', length(wav3_cc));
printf('  (+) CC-Features %d\n', length(dat3_ccff));


if (opt_spam)
	audio_wd_out  = sprintf('seq_%.4d/wd_%.4d_us_fine.wav', seq, num);
	audio_ag_out  = sprintf('seq_%.4d/wd_%.4d_ag_fine.wav', seq, num);
	if (opt_nolg == 0)
		audio_lg_out  = sprintf('seq_%.4d/wd_%.4d_lg_fine.wav', seq, num);
	end
	audio_cc_out  = sprintf('seq_%.4d/wd_%.4d_cc_fine.wav', seq, num);

	wavwrite(wav3_wd, std_rate, audio_wd_out);
	wavwrite(wav3_ag, std_rate, audio_ag_out);
	if (opt_nolg == 0)
		wavwrite([wav3_lg dat3_lg], std_rate, audio_lg_out);
	end
	wavwrite(wav3_cc, std_rate, audio_cc_out);
end

%data = {};
data.US.spc = wav3_wd;
data.US.fea = dat3_usff;
%data.US.or.spc = 48000;
%data.US.or.fea = 25;

data.AG.spc = wav3_ag;
data.AG.amp = dat3_agamp;
data.AG.pos = dat3_agpos;
%data.AG.or.spc = 16000;
%data.AG.or.amp = 200;
%data.AG.or.pos = 200;

if (opt_nolg == 0)
	data.LG.egg = dat3_lg;
	data.LG.spc = wav3_lg;
else
	data.LG.egg = zeros(size(wav3_wd));
	data.LG.spc = zeros(size(wav3_wd));
end
%data.LG.or.spc = 16000;
%data.LG.or.egg = 16000;

data.CC.fea = dat3_ccff;
data.CC.spc = wav3_cc;
%data.CC.or.spc = 48000;
%data.CC.or.fea = 25;

data.misc.time = [0:1/std_rate:(length(data.US.spc) - 1)/std_rate];
data.misc.rate = std_rate;
data.misc.length = length(wav3_wd);
