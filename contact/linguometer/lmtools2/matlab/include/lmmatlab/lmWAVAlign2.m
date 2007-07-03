% Copyright (C) 2007 Michele Tavella <michele@liralab.it>
%                    Paul Fitzpatrick <paulfitz@liralab.it>
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

function offset = lmWAVAlign2(wordNumber, do_invert)


file_wd = sprintf('wd_%04d_us.wav',wordNumber);
file_cc_est = sprintf('wd_%04d_cc_est.wav',wordNumber);

[wav0 rate0] = wavread(file_wd);
[wav1 rate1] = wavread(file_cc_est);

printf('[lmWAVAlign] WD file:       %s\n', file_wd);
printf('[lmWAVAlign] CC (est) file: %s\n', file_cc_est);
if (rate0 ~= rate1)
  disp('Need to add code to resample one of the audio sources, sorry')
end

rate = rate0;
% zap click peaks
if (do_invert == 0)
    [ignore peak] = max(wav0(:,2));
    wav0(1:(peak+round(rate*0.1)),:) = 0;  % null out click at start
    [ignore peak] = min(wav0(:,2));
    wav0((peak-round(rate*0.1)):length(wav0),:) = 0;  % null out tail
else
    [ignore peak] = max(wav0(:,2));
    wav0((peak-round(rate*0.1)):length(wav0),:) = 0;  % null out tail
    [ignore peak] = min(wav0(:,2));
    wav0(1:(peak+round(rate*0.1)),:) = 0;  % null out click at start
end


% forget about stereo for comparison
wav0 = wav0(:,2);
wav1 = wav1(:,2);

% compute something energy-like -- easier to compare
sig0 = abs(wav0);
sig1 = abs(wav1);

%sig0 = 20*log(sig0.^2 + 1);
%sig1 = 20*log(sig1.^2 + 1);
f = ones(round(rate*0.1),1);  % really dumb smoothing
%f = ones(round(rate*0.01),1);  % really dumb smoothing
sig0 = filter2(f,sig0,'same');
sig1 = filter2(f,sig1,'same');

figure(1);
plot(sig0); axis tight; grid on;
figure(2);
plot(sig1); axis tight; grid on;

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
