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
