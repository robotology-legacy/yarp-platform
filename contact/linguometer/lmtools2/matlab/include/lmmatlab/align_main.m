clear all;

opt_plot = 1;
opt_invert = 0;
seq = 0;
num = 60;

std_rate = 48000;
filter_fast = ones(round(std_rate*0.01),1);
filter_slow = ones(round(std_rate*0.1),1);
if (0)
	mtPlotFFT(filter_fast, 48e3, 10);
end

audio_wd  = sprintf('seq_%.4d/wd_%.4d_us.wav', seq, num);
audio_ag  = sprintf('seq_%.4d/wd_%.4d_ag.wav', seq, num);
audio_lg  = sprintf('seq_%.4d/wd_%.4d_lg.wav', seq, num);
audio_cc  = sprintf('seq_%.4d/wd_%.4d_cc.wav', seq, num);


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
wav_lg = owav_lg(:, 1);
wav_lg = resample(wav_lg, std_rate, orate_lg);
% Select the left CC-Speech channel
wav_cc  = owav_cc(:, 1);
wav_cc = owav_cc(:, 1);

std_zap0 = max([zap0_wd zap0_ag]);
std_zap1 = min([zap1_wd zap1_ag]);

sig_wd  = filter2(filter_fast, abs(wav_wd), 'same');
sig_ag  = filter2(filter_fast, abs(wav_ag), 'same');
sig_lg  = filter2(filter_fast, abs(wav_lg), 'same');
sig_cc  = filter2(filter_fast, abs(wav_cc), 'same');
sig_cc  = filter2(filter_fast, abs(wav_cc), 'same');

printf('Computing offset values:\n');
offset_ag  = align_lag(sig_wd, sig_ag);
offset_lg  = align_lag(sig_wd, sig_lg);
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

wav2_ag  = lagmatrix(wav_ag,  offset_ag);
wav2_lg  = lagmatrix(wav_lg,  offset_lg);
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


% Few index values...
iwd = 1;
iag = 2;
ilg = 3;
icc = 4;


% remove NaN values
wav3_wd = wav_wd;
wav3_ag = align_crop(wav2_ag, offset_ag);
wav3_lg = align_crop(wav2_lg, offset_lg);
wav3_cc = align_crop(wav2_cc, offset_cc);

lengths = zeros(4, 1);
lengths(iwd) = length(wav3_wd);
lengths(iag) = length(wav3_ag);
lengths(ilg) = length(wav3_lg);
lengths(icc) = length(wav3_cc);

if (std_zap1 <= min(lengths))
	std_lenght = std_zap1;
else
	std_lenght = min(lengths);
end

wav3_wd = wav3_wd(std_zap0:std_lenght);
wav3_ag = wav3_ag(std_zap0:std_lenght);
wav3_lg = wav3_lg(std_zap0:std_lenght);
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
	plot(sig_wd, 'r');
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

	subplot(4, 2, 1);
	plot(time, wav3_wd, 'r');
	grid on;
	axis tight;
	ylabel('US-Speech');
	title('Alignment reference tracks: speech');
	
	subplot(4, 2, 3);
	plot(time, wav3_ag, 'b');
	grid on;
	axis tight;
	ylabel('AG-Speech');
	
	subplot(4, 2, 5);
	plot(time, wav3_lg, 'k');
	grid on;
	axis tight;
	ylabel('LG-Speech');
	
	subplot(4, 2, 7);
	plot(time, wav3_cc, 'g');
	grid on;
	axis tight;
	ylabel('CC-Speech');
	xlabel('Time [s]');
		
	subplot(4, 2, 2);
	plot(time, filter2(filter_slow, abs(wav3_wd), 'same'), 'r');
	grid on;
	axis tight;
	title('Alignment reference tracks: smooth energy-like');
	
	subplot(4, 2, 4);
	plot(time, filter2(filter_slow, abs(wav3_ag), 'same'), 'b');
	grid on;
	axis tight;
	
	subplot(4, 2, 6);
	plot(time, filter2(filter_slow, abs(wav3_lg), 'same'), 'k');
	grid on;
	axis tight;
	
	subplot(4, 2, 8);
	plot(time, filter2(filter_slow, abs(wav3_cc), 'same'), 'g');
	grid on;
	axis tight;
	xlabel('Time [s]');
end


audio_wd_out  = sprintf('seq_%.4d/wd_%.4d_us_fine.wav', seq, num);
audio_ag_out  = sprintf('seq_%.4d/wd_%.4d_ag_fine.wav', seq, num);
audio_lg_out  = sprintf('seq_%.4d/wd_%.4d_lg_fine.wav', seq, num);
audio_cc_out  = sprintf('seq_%.4d/wd_%.4d_cc_fine.wav', seq, num);

wavwrite(wav3_wd, std_rate, audio_wd_out);
wavwrite(wav3_ag, std_rate, audio_ag_out);
wavwrite(wav3_lg, std_rate, audio_lg_out);
wavwrite(wav3_cc, std_rate, audio_cc_out);
