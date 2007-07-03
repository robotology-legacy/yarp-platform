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

function [PCM T FFT F] = mtPlotWAV (file, f1, f2);
%clear all;

opt_fft     = false;
opt_ffthalf = true;


PCM = importdata (file);
PCM.data = PCM.data(20.6*16000:22*16000, :);
%Fs = PCM.fs;
%PCM = PCM (1:48000*2, :);
ch = min (size (PCM.data));

Smin = 1;
Smax = length (PCM.data);
Amin = -2^15; 
Amax = +2^15 - 1;

if (ch == 2)
	audioL = PCM.data (:, 1);
	audioR = PCM.data (:, 2);

	T = [0:1/48e3:(length(audioL) - 1)/48e3];
	Tmin = 0;
	Tmax = T (Smax);

	figure (f1);
	subplot (2, 1, 1);
	plot (T, audioL, 'k');
	axis ([Tmin Tmax Amin Amax]);
	axis tight;
	grid on;
	title ('Speech');
	%xlabel ('Time (s)');
	ylabel ('Amplidude (16 bits)');

	subplot (2, 1, 2);
	plot (T, audioR, 'r');
	grid on;
	%axis ([Tmin Tmax Amin Amax]);
	axis tight;
	title ('Lx + Gx');
	xlabel ('Time (s)');
	ylabel ('Amplidude (16 bits)');
elseif (ch == 1)
	audioM = PCM.data;
	
	T = [0:1/48e3:(length(audioM) - 1)/48e3];
	Tmin = 0;
	Tmax = T (Smax);

	m_figure (f1);
	plot (T, audioM, 'r');
	%axis ([Tmin Tmax Amin Amax]);
	axis tight;
	grid on;
	title ('Mono Channel');
	xlabel ('Time (s)');
	ylabel ('Amplidude (16 bits)');
end;

if (opt_fft)
	L = length (PCM.data);
	%PCM.fs = 48000;

	m_figure (f2);
	if (ch == 2)
		fftR = fftshift (fft(audioR))/PCM.fs;
		fftL = fftshift (fft(audioL))/PCM.fs;
		df = 1 / (L / PCM.fs);

		if (floor (L/2) == L/2)
			f = [-L/2+[0:L-1]]*df;	
		else
			f = [-(L-1)/2+[0:L-1]]*df;	
		end
		
		f0 = floor (length (f) / 2);
		f1 = length (f);

		subplot (2, 1, 1);
		if (opt_ffthalf)
			plot (f (f0:f1), abs (fftL(f0:f1)), 'k');
		else
			plot (f, abs (fftL), 'k');
		end
		grid on;
		axis tight;
		title ('Left Channel');
		xlabel ('Frequency (Hz)');
		ylabel ('Abs');

		subplot (2, 1, 2);
		if (opt_ffthalf)
			plot (f (f0:f1), abs (fftR(f0:f1)), 'r');
		else
			plot (f, abs (fftR), 'r');
		end
		grid on;
		axis tight;
		title ('Right Channel');
		xlabel ('Frequency (Hz)');
		ylabel ('Abs');

		FFT.fftR = fftR;
		FFT.fftL = fftL;
		F = f;
	elseif (ch == 1)
		fftM = fftshift (fft(audioM))/PCM.fs;
		df = 1 / (L / PCM.fs);

		if (floor (L/2) == L/2)
			f = [-L/2+[0:L-1]]*df;	
		else
			f = [-(L-1)/2+[0:L-1]]*df;	
		end
		
		f0 = floor (length (f) / 2);
		f1 = length (f);

		if (opt_ffthalf)
			plot (f (f0:f1), abs (fftM(f0:f1)), 'k');
		else
			plot (f, abs (fftM), 'k');
		end
		grid on;
		axis tight;
		title ('Mono Channel');
		xlabel ('Frequency (Hz)');
		ylabel ('Abs');
		FFT.fftM = fftM;
		F = f;
	end
	end
end
