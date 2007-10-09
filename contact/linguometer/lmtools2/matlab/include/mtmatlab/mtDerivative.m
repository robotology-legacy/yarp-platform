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
% function dx = mtDerivativeWin(x)
%
% mtDerivativeWin is an implementation of the code found in:
%    "The HTK Book (for HTK Version 3.3)"
%    by Steve Young, Gunnar Evermann, Mark Gales
%    Thomas Hain, Dan Kershaw, Gareth Moore, Julian Odell, 
%    Dave Ollason, Dan Povey, Valtcho Valtchev and Phil Woodland

function dx = mtDerivative(x, wsize)

if (wsize <= 0)
	% Correct x length so that numel(x) is an odd number
	if(mod(length(x),2) == 0)
		x = x(1:length(x)-1);
	end
	% Select s0, aka the center of the window
	s0 = (length(x)-1)/2;
	wsize = s0-1;

	% Compute dx in s0 
	dx_num = 0;
	dx_den = 0;
	for alpha = 1:wsize
		dx_num = dx_num + alpha*(x(s0+alpha) - x(s0-alpha));
		dx_den = dx_den + alpha^2;
	end
	dx_den = 2*dx_den;
	dx = dx_num/dx_den;
else
	pad_s0 = ones(1,wsize) * x(1);
	pad_s1 = ones(1,wsize) * x(length(x));
	x = [pad_s0 x pad_s1];
	for s0 = [(1+wsize):1:(length(x)-wsize)]
		dx_num = 0;
		dx_den = 0;
		for alpha = 1:wsize
			dx_num = dx_num + alpha*(x(s0+alpha) - x(s0-alpha));
			dx_den = dx_den + alpha^2;
		end
		dx_den = 2*dx_den;
		dx(s0) = dx_num/dx_den;
	end
	dx = dx((1+wsize):(length(x)-wsize));
end
