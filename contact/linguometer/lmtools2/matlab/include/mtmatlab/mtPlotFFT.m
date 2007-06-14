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

function mtPlotFFT (X, sf, bfig)

mtSimpleFig(bfig);
L = length(X);
NFFT = 2^nextpow2(L)
Y = fft(X,NFFT)/L;
f = sf/2*linspace(0,1,NFFT/2);
plot(f,2*abs(Y(1:NFFT/2))) 

grid on;
axis tight;
string_title = sprintf('FFT\nF_s=%d Hz', sf);
title (string_title);
xlabel ('Frequency (Hz)');
ylabel ('Module');
