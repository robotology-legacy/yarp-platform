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

function [wav zap0 zap1] = lmpkgZap(wav, rate, do_invert, do_sat)

if (do_sat == 0)
	if (do_invert == 0)
		[ignore peak] = max(wav);
		zap0 = peak+round(rate*0.1);
		wav(1:zap0,:) = 0;

		[ignore peak] = min(wav);
		zap1 = peak-round(rate*0.1);
		wav(zap1:length(wav),:) = 0; 
	else
		[ignore peak] = max(wav);
		zap0 = peak-round(rate*0.1);
		wav(zap0:length(wav),:) = 0;

		[ignore peak] = min(wav);
		zap1 = peak+round(rate*0.1);
		wav(1:zap1,:) = 0;
	end
else
	if (do_invert == 0)
		wav_t = wav(1:round(length(wav)/4));
		[ignore peak] = max(wav_t);
		zap0 = peak+round(rate*0.1)
		wav(1:zap0,:) = 0;

		wav_t = wav(round(length(wav)*3/4):length(wav));
		[ignore peak] = min(wav_t);
		peak = peak + round(length(wav)*3/4); 
		zap1 = peak-round(rate*0.1)
		wav(zap1:length(wav),:) = 0; 
	else
		wav_t = wav(1:round(length(wav)/4));
		[ignore peak] = max(wav_t);
		zap0 = peak-round(rate*0.1);
		wav(zap0:length(wav),:) = 0;
		
		wav_t = wav(round(length(wav)*3/4):length(wav));
		[ignore peak] = min(wav_t);
		peak = peak + round(length(wav)*3/4); 
		zap1 = peak+round(rate*0.1);
		wav(1:zap1,:) = 0;
	end
end
