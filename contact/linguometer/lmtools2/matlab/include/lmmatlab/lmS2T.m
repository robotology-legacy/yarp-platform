function t = lmS2T (AG, samples)
% Usage: t = mS2T (AG, samples)
% Where: AG        profile
%        samples   number of samples according
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
t = samples / (AG.pars.sr * tf);
