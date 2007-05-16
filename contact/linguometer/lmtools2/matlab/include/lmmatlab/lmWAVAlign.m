% Coded by Paul Fitzpatrick (paulfitz@liralab.it)
% Integrated in lmtools by Michele Tavella (michele@liralab.it)
function offset = lmWAVAlign(file_wd, file_cc_est)

%file_wd = sprintf('wd_%04d.wav',wordNumber);
%file_cc_est = sprintf('wd_%04d_cc_est.wav',wordNumber);

[wav0 rate0] = wavread(file_wd);
[wav1 rate1] = wavread(file_cc_est);

printf('[lmWAVAlign] WD file:       %s\n', file_wd);
printf('[lmWAVAlign] CC (est) file: %s\n', file_cc_est);
if (rate0 ~= rate1)
  disp('Need to add code to resample one of the audio sources, sorry')
end

rate = rate0;
% zap click peaks
[ignore peak] = max(wav0(:,2));
wav0(1:(peak+round(rate*0.1)),:) = 0;  % null out click at start
[ignore peak] = min(wav0(:,2));
wav0((peak-round(rate*0.1)):length(wav0),:) = 0;  % null out tail

% forget about stereo for comparison
wav0 = wav0(:,2);
wav1 = wav1(:,2);

% compute something energy-like -- easier to compare
sig0 = abs(wav0);
sig1 = abs(wav1);

% Paul's Watt-Friendly choice
%f_factor = 0.01; 
% Michele Coffee-break friendly choice:
f_factor = 0.1; 
f = ones(round(rate*f_factor),1);  % really dumb smoothing
sig0 = filter2(f,sig0,'same');
sig1 = filter2(f,sig1,'same');

[c lag] = xcorr(sig0, sig1);
[ignore winner] = max(c);
%offset = -lag(winner);
offset = lag(winner);
disp(['myans = ' num2str(offset)]);


%if (wantView)
%	%sig1b = shift(sig1,offset);
%	%sig1b = lagmatrix(sig1,offset);
%	figure(1);
%	plot(sig0, 'r');
%	hold on;
%	plot(sig1, 'k');
%	hold off;
%	title('unaligned');
%
%	figure(2);
%	%plot(sig0, 'r');
%	hold on;
%	plot(sig1b, 'k');
%	hold off;
%	title('aligned');
%	disp(['offset in samples is: ' num2str(offset)]);
%end;
