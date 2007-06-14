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

function offset = lmWAVAlign(file_wd, file_cc_est, do_invert, channel)


do_plot = 0;

%US<-->CC channel=2
%US<-->LG channel=1

[wav0 rate0] = wavread(file_wd);
[wav1 rate1] = wavread(file_cc_est);

printf('[lmWAVAlign] WD file:       %s\n', file_wd);
printf('[lmWAVAlign] CC (est) file: %s\n', file_cc_est);
rate_old = 0;
if (rate0 ~= rate1)
  %disp('Need to add code to resample one of the audio sources, sorry')
  rate_old = rate1;
  wav1 = resample(wav1, rate0, rate1);
end

rate = rate0;
% zap click peaks
%[ignore peak] = max(wav0(:,2));
%wav0(1:(peak+round(rate*0.1)),:) = 0;  % null out click at start
%[ignore peak] = min(wav0(:,2));
%wav0((peak-round(rate*0.1)):length(wav0),:) = 0;  % null out tail
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
wav1 = wav1(:,channel);

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
if(rate_old ~= 0)
    offset = round(offset * rate_old / rate);
end
disp(['myans = ' num2str(offset)]);

if(do_plot)
figure(1); 
plot(sig0, 'b'); axis tight; grid on; title('Target');
figure(2); 
plot(sig1, 'r'); axis tight; grid on; title('Source');
figure(3); 
plot(sig0, 'b'); hold on; plot(lagmatrix(sig1,offset), 'r'); axis tight; grid on; title('Alignment');
end
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
