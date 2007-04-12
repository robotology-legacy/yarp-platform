function samples = lmT2S (AG, t)
% Usage: samples = mClockToSamples (AG, t)
% Where: AG        profile
%        time      time instant according 
%                  to AG.pars.tu time unit

switch (AG.pars.tu)
	case 'ds'
		tf = 1e-1;
	case 'cs'
		tf = 1e-2;
	case 'ms'
		tf = 1e-3;
	case 's'
		tf = 1;
end


samples = t * tf * AG.pars.sr;

if (samples == 0)
	samples = 1;
end
