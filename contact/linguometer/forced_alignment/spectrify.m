function [str nFs] = spectrify(x,Fs)

#plot(v(dom));
top = 8000; # magnitude in range 0<f<=8000 Hz.

datawin = 10;   # was: 40 ms data window
step = 5;    # was: one spectral slice every 5 ms


more off;
   step = fix(step*Fs/1000)
   nFs = Fs/step;

   window = fix(datawin*Fs/1000)
#   plot(x(1:window))

   fftnbase = 2^nextpow2(window); # next highest power of 2

   pretty = 1;

   fftn = fftnbase*pretty;
   [S, f, t] = specgram(x, fftn, Fs, window, window-step);
   S = S(2:fftn*top/Fs,:);
   ang = angle(S);
   S = abs(S);
   S = S/max(S(:));           # normalize magnitude so that max is 0 dB.
   val = S;
   str = val;
   S = max(S, 10^(-40/10));   # clip below -40 dB.
   S = min(S, 10^(-3/10));    # clip above -3 dB.
   vval = -flipud(log(S));
   #imagesc(vval);   # display in log scale
   str = vval;

