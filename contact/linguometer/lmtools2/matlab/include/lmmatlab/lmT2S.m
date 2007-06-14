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
