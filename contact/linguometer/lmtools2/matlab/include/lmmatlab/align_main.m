clear all;

seq = 0;
num = 70;

std_rate = 48000;
opt_plot = 1;
opt_invert = 0;

% Filters used for smoothing the signals
filter_fast = ones(round(std_rate*0.01),1);
filter_slow = ones(round(std_rate*0.1),1);
%filter_fast = hamming(round(std_rate*0.01));
%filter_slow = hamming(round(std_rate*0.1));
%filter_fast = bartlett(round(std_rate*0.01));
%filter_slow = bartlett(round(std_rate*0.1));
if (0)
	mtPlotFFT(filter_fast, 48e3, 10);
end


% Load audio files (reference)
audio_wd = sprintf('seq_%.4d/wd_%.4d_us.wav', seq, num);
audio_ag = sprintf('seq_%.4d/wd_%.4d_ag.wav', seq, num);
audio_lg = sprintf('seq_%.4d/wd_%.4d_lg.wav', seq, num);
audio_cc = sprintf('seq_%.4d/wd_%.4d_cc.wav', seq, num);
[owav_wd  orate_wd]  = wavread(audio_wd);
[owav_ag  orate_ag]  = wavread(audio_ag);
[owav_lg  orate_lg]  = wavread(audio_lg);
[owav_cc  orate_cc]  = wavread(audio_cc);

% Select the right channel (US-Speech)
wav_wd = owav_wd(:, 2);
[wav_wd zap0_wd zap1_wd] = align_zap(wav_wd, std_rate, opt_invert);

% Resample the AG-Speech data (16-->48kHz)
wav_ag = resample(owav_ag, std_rate, orate_ag);
rate_ag = std_rate;
[wav_ag zap0_ag zap1_ag] = align_zap(wav_ag, std_rate, opt_invert);

% Select the left channel (LG-Speech)
wav_lg = resample(owav_lg, std_rate, orate_lg);
dat_lg = wav_lg(:, 2);
wav_lg = wav_lg(:, 1);

% Select the left CC-Speech channel
wav_cc  = owav_cc(:, 1);
wav_cc = owav_cc(:, 1);

std_zap0 = max([zap0_wd zap0_ag]);
std_zap1 = min([zap1_wd zap1_ag]);


% Load data files (reference)
file_agamp = sprintf('seq_%.4d/wd_%.4d_ag.amp', seq, num); 
odat_agamp = importdata(file_agamp);
dat_agamp = resample(odat_agamp, std_rate, 200);


sig_wd  = filter2(filter_fast, abs(wav_wd), 'same');
sig_ag  = filter2(filter_fast, abs(wav_ag), 'same');
sig_lg  = filter2(filter_fast, abs(wav_lg), 'same');
sig_cc  = filter2(filter_fast, abs(wav_cc), 'same');
sig_cc  = filter2(filter_fast, abs(wav_cc), 'same');

printf('Computing offset values:\n');
offset_ag = align_lag(sig_wd, sig_ag);
offset_lg = align_lag(sig_wd, sig_lg);
offset_cc = align_lag(sig_wd, sig_cc);
printf('    WD   %d (reference)\n', 0);
printf('    AG   %d\n', offset_ag);
printf('    LG   %d\n', offset_lg);
printf('    CCt  %d\n', offset_cc);

printf('Lenght of signals\n');
printf('    WD   %d\n', length(wav_wd));
printf('    AG   %d\n', length(wav_ag));
printf('    LG   %d\n', length(wav_lg));
printf('    CCt  %d\n', length(wav_cc));

wav2_ag = lagmatrix(wav_ag, offset_ag);
dat2_agamp = lagmatrix(dat_agamp, offset_lg);
wav2_lg = lagmatrix(wav_lg, offset_lg);
dat2_lg = lagmatrix(dat_lg, offset_lg);
wav2_cc = lagmatrix(wav_cc, offset_cc);


if (opt_plot)
	mtSimpleFig(1);
	plot(sig_wd, 'r');
	hold on;
	plot(sig_ag, 'b');
	plot(sig_lg, 'k');
	plot(sig_cc, 'g');
	hold off;
	grid on;
	axis tight;
	xlabel('samples');
	ylabel('energy');
	title('LM Fine Alignment: before');
	legend('WD', sprintf('AG (%d)', offset_ag), sprintf('LG (%d)', offset_lg), sprintf('CCt (%d)', offset_cc));
end


% remove NaN values
wav3_wd = wav_wd;
wav3_ag = align_crop(wav2_ag, offset_ag);
dat3_agamp = align_crop(dat2_agamp, offset_ag);
wav3_lg = align_crop(wav2_lg, offset_lg);
dat3_lg = align_crop(dat2_lg, offset_lg);
wav3_cc = align_crop(wav2_cc, offset_cc);

lengths = zeros(4, 1);
lengths(1) = length(wav3_wd);
lengths(2) = length(wav3_ag);
lengths(3) = length(wav3_lg);
lengths(4) = length(wav3_cc);

if (std_zap1 <= min(lengths))
	std_lenght = std_zap1;
else
	std_lenght = min(lengths);
end

wav3_wd = wav3_wd(std_zap0:std_lenght);
wav3_ag = wav3_ag(std_zap0:std_lenght);
dat3_agamp = dat3_agamp(std_zap0:std_lenght, :);
wav3_lg = wav3_lg(std_zap0:std_lenght);
dat3_lg = dat3_lg(std_zap0:std_lenght);
wav3_cc = wav3_cc(std_zap0:std_lenght);

sig3_wd = filter2(filter_fast, abs(wav3_wd), 'same');
sig3_ag = filter2(filter_fast, abs(wav3_ag), 'same');
sig3_lg = filter2(filter_fast, abs(wav3_lg), 'same');
sig3_cc = filter2(filter_fast, abs(wav3_cc), 'same');

printf('Computing offset values:\n');
offset3_ag = align_lag(sig3_wd, sig3_ag);
offset3_lg = align_lag(sig3_wd, sig3_lg);
offset3_cc = align_lag(sig3_wd, sig3_cc);
printf('    WD   %d (reference)\n', 0);
printf('    AG   %d\n', offset3_ag);
printf('    LG   %d\n', offset3_lg);
printf('    CCt  %d\n', offset3_cc);


if (opt_plot)
	mtSimpleFig(2);
	plot(sig3_wd, 'r');
	hold on;
	plot(sig3_ag, 'b');
	plot(sig3_lg, 'k');
	plot(sig3_cc, 'g');
	hold off;
	grid on;
	axis tight;
	xlabel('samples');
	ylabel('energy');
	title('LM Fine Alignment: before');
	legend('WD', 'AG', 'LG', 'CCt');
end

if (opt_plot)
	time = [0:1/std_rate:(length(wav3_wd) - 1)/std_rate];
	mtSimpleFig(3);

	subplot(4, 3, 1);
	plot(time, wav3_wd, 'r');
	grid on;
	axis tight;
	ylabel('US-Speech');
	title('Alignment reference tracks: speech');
	
	subplot(4, 3, 4);
	plot(time, wav3_ag, 'b');
	grid on;
	axis tight;
	ylabel('AG-Speech');
	
	subplot(4, 3, 7);
	plot(time, wav3_lg, 'k');
	grid on;
	axis tight;
	ylabel('LG-Speech');
	
	subplot(4, 3, 10);
	plot(time, wav3_cc, 'g');
	grid on;
	axis tight;
	ylabel('CC-Speech');
	xlabel('Time [s]');
		
	subplot(4, 3, 2);
	plot(time, filter2(filter_slow, abs(wav3_wd), 'same'), 'r');
	grid on;
	axis tight;
	title('Alignment reference tracks: smooth energy-like');
	
	subplot(4, 3, 5);
	plot(time, filter2(filter_slow, abs(wav3_ag), 'same'), 'b');
	grid on;
	axis tight;
	
	subplot(4, 3, 8);
	plot(time, filter2(filter_slow, abs(wav3_lg), 'same'), 'k');
	grid on;
	axis tight;
	
	subplot(4, 3, 11);
	plot(time, filter2(filter_slow, abs(wav3_cc), 'same'), 'g');
	grid on;
	axis tight;
	xlabel('Time [s]');

	subplot(4, 3, 3);
	mesh(resample(dat3_agamp, 25, std_rate));
	grid on;
	axis tight;
	ylabel('Samples');
	xlabel('Current');
	zlabel('AG-Amp');
	title('Data tracks');

	subplot(4, 3, 9);
	plot(time, dat3_lg, 'k');
	grid on;
	axis tight;
	ylabel('LG-Data');
	xlabel('Time [s]');
end


audio_wd_out  = sprintf('seq_%.4d/wd_%.4d_us_fine.wav', seq, num);
audio_ag_out  = sprintf('seq_%.4d/wd_%.4d_ag_fine.wav', seq, num);
audio_lg_out  = sprintf('seq_%.4d/wd_%.4d_lg_fine.wav', seq, num);
audio_cc_out  = sprintf('seq_%.4d/wd_%.4d_cc_fine.wav', seq, num);

wavwrite(wav3_wd, std_rate, audio_wd_out);
wavwrite(wav3_ag, std_rate, audio_ag_out);
wavwrite([wav3_lg dat3_lg], std_rate, audio_lg_out);
wavwrite(wav3_cc, std_rate, audio_cc_out);
