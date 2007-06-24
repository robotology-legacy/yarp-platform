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
%
%function [wav zap0 zap1] = lmpkgZap(wav, rate, do_invert, do_sat)

function [wav zap0 zap1] = lmpkgZap(wav, rate, do_sat)
ol = length(wav);
th = 0.60;
cf = 0.1;

zap_bug_ll = 1/5;
zap_bug_ul = 1 - zap_bug_ll;

if (do_sat == 0)
	[ignore peak] = max(wav);
	zap0 = peak+round(rate*cf);
	wav(1:zap0,:) = 0;

	[ignore peak] = min(wav);
	zap1 = peak-round(rate*cf);
	wav(zap1:length(wav),:) = 0; 
else
	[zap_th0 zap_th1] = lmpkgPeaksTh(wav, th);
	[zap_mm0 zap_mm1] = lmpkgPeaksMinMax (wav, rate, zap_bug_ll, zap_bug_ul, cf);

	if (zap_th0 <= 0)
		zap0 = zap_mm0; 
	else
		zap0 = zap_th0; 
	end
	if (zap_th1 <= 0)
		zap1 = zap_mm1; 
	else
		zap1 = zap_th1; 
	end

	lmpkgPeaksPlot (2222, wav, zap0, zap1);
	zap0 = zap0 + round(rate*cf);
	zap1 = zap1 - round(rate*cf);
	lmpkgPeaksPlot (2222, wav, zap0, zap1);
	drawnow;

	wav(1:zap0,:) = 0;
	wav(zap1:length(wav),:) = 0; 
end
printf('[lmpkgZap] Zapping between %d and %d/%d\n', zap0, zap1, ol);




function [zap_raw_0 zap_raw_1] = lmpkgPeaksTh (wav, th)
zap_raw_0 = 0;
zap_raw_1 = 0;

pstot = 0;
ps0 = 0;
ps1 = 0;
for s = 1:round(length(wav)/2)
	if(wav(s) > th)
		pstot = pstot + 1;
		ps1 = s;

		if (pstot == 0)
			ps0 = s;
		end
	elseif (pstot > 60)
		zap_raw_0 = ps1;
		printf('[lmpkgFetchPeaks] WD-Start peak found: %d-%d\n', 1, ps1);
		break;
	end
end

pstot = 0;
ps0 = 0;
ps1 = 0;
for s = round(length(wav)-length(wav)/2):length(wav)
	s = length(wav) - s + round(length(wav)-length(wav)/2);
	if(wav(s) < -th)
		pstot = pstot + 1;
		ps0 = s;

		if (pstot == 0)
			ps1 = s;
		end
	elseif (pstot > 60)
		zap_raw_1 = ps0;
		printf('[lmpkgFetchPeaks] WD-Stop peak found: %d-%d\n', ps0, length(wav));
		break;
	end
end


function [zap0 zap1] = lmpkgPeaksMinMax (wav, rate, zap_bug_ll, zap_bug_ul, cf)
wav_t = wav(1:round(length(wav)*zap_bug_ll));
[ignore peak] = max(wav_t);
zap0 = peak+round(rate*cf);
wav(1:zap0,:) = 0;

wav_t = wav(round(length(wav)*zap_bug_ul):length(wav));
[ignore peak] = min(wav_t);
peak = peak + round(length(wav)*zap_bug_ul); 
zap1 = peak-round(rate*cf);
wav(zap1:length(wav),:) = 0; 


function lmpkgPeaksPlot (fig, wav, zap0, zap1)
mtSimpleFig(fig);
plot(wav);
axis tight;
hold on;
plot(zap0, +0.50, 'ro', 'LineWidth', 2)
plot(zap1, -0.50, 'ro', 'LineWidth', 2)
hold off;
grid on;
title('DumbTM Peak Detector');
