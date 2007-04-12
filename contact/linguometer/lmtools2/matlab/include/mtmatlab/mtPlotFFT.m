function mtPlotFFT (X, sf, bfig)

opt_ffthalf = 1;

mtSimpleFig(bfig);
%(AG, file, f1, f2);
%clear all;
L = length(X);
F = fftshift(fft(X))/sf;
df = 1 / (L / sf);

if (floor (L/2) == L/2)
	f = [-L/2+[0:L-1]]*df;	
else
	f = [-(L-1)/2+[0:L-1]]*df;	
end


if (opt_ffthalf)
	f0 = floor (length (f) / 2);
	f1 = length (f);
	plot (f (f0:f1), abs (F(f0:f1)), 'r');
else
	plot (f, abs (F), 'r');
end
grid on;
axis tight;
string_title = sprintf('FFT\nF_s=%d Hz', sf);
title (string_title);
xlabel ('Frequency (Hz)');
ylabel ('Module');
