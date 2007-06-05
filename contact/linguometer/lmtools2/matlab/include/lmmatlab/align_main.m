clear all;

opt_plot = 1;
opt_invert = 0;
seq = 0;
num = 0;

std_rate = 48000;
filter = ones(round(std_rate*0.2),1);

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
wav_wd = align_zap(wav_wd, std_rate, opt_invert);
% Resample the AG-Speech data (16-->48kHz)
wav_ag = resample(owav_ag, std_rate, orate_ag);
rate_ag = std_rate;
wav_ag = align_zap(wav_ag, std_rate, opt_invert);
% Select the left channel (LG-Speech)
wav_lg = owav_lg(:, 1);
wav_lg = resample(wav_lg, std_rate, orate_lg);
% Select the left CC-Speech channel
wav_cc  = owav_cc(:, 1);
wav_cc = owav_cc(:, 1);

sig_wd   = filter2(filter, abs(wav_wd),  'same');
sig_ag   = filter2(filter, abs(wav_ag),  'same');
sig_lg   = filter2(filter, abs(wav_lg),  'same');
sig_cc   = filter2(filter, abs(wav_cc),  'same');
sig_cc  = filter2(filter, abs(wav_cc), 'same');

printf('Computing offset values:\n');
offset_ag  = align_lag(sig_wd, sig_ag);
offset_lg  = align_lag(sig_wd, sig_lg);
offset_cc = align_lag(sig_wd, sig_cc);
printf('    WD   %d\n', 0);
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

% ... in order to handle vectors, such
% as the length vector...
lengths = zeros(4, 1);
lengths(iwd) = length(wav_wd);
lengths(iag) = length(wav_ag);
lengths(ilg) = length(wav_lg);
lengths(icc) = length(wav_cc);

% ...and the offset vector
offsets = zeros(4, 1);
offsets(iwd) = 0;
offsets(iag) = offset_ag;
offsets(ilg) = offset_lg;
offsets(icc) = offset_cc;

std_lenght = min(lengths);
std_offset = min(offsets);

% remove NaN values
wav3_ag = align_crop(wav2_ag, offset_ag);
wav3_lg = align_crop(wav2_lg, offset_lg);
wav3_cc = align_crop(wav2_cc, offset_cc);










if (1)
	sig2_ag   = filter2(filter, abs(wav3_ag),  'same');
	sig2_lg   = filter2(filter, abs(wav3_lg),  'same');
	sig2_cc  = filter2(filter, abs(wav3_cc), 'same');

	offset_ag  = align_lag(sig_wd, sig2_ag)
	offset_lg  = align_lag(sig_wd, sig2_lg)
	offset_cc = align_lag(sig_wd, sig2_cc)

	mtSimpleFig(2);
	plot(sig_wd, 'r');
	hold on;
	plot(sig2_ag, 'b');
	plot(sig2_lg, 'k');
	plot(sig2_cc, 'g');
	hold off;
	grid on;
	axis tight;
	xlabel('samples');
	ylabel('energy');
	title('LM Fine Alignment: before');
	legend('WD', 'AG', 'LG', 'CCt');
end
