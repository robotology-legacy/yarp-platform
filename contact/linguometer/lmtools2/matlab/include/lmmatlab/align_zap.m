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

function [wav zap0 zap1] = align_zap(wav, rate, do_invert)
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
