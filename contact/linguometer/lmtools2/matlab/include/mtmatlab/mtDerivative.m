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
% function dx = mtDerivative (x, wsize)
%
% For example:
% figure(1); clf;
% t = [1:0.1:20];
% x = sin(t) +  0.25*rand(size(t));
% plot(t,x,'r'); 
% grid on; hold on; axis tight;
% title('mtDerivative - Example');
% plot(t,cos(t),'b');
% dx = mtNormalize(mtDerivative(x, 10),0,2)-1 ;
% plot(t,dx,'k');
% legend('X(t)=sin(t)+N(t)', 'Y=cos(t)', 'd/dt(X(t))');
%
% mtDerivative is an implementation of the code found in:
%    "The HTK Book (for HTK Version 3.3)"
%    by Steve Young, Gunnar Evermann, Mark Gales
%    Thomas Hain, Dan Kershaw, Gareth Moore, Julian Odell, 
%    Dave Ollason, Dan Povey, Valtcho Valtchev and Phil Woodland

function dx = mtDerivative (x, wsize)

pad_s0 = ones(1,wsize) * x(1);
pad_s1 = ones(1,wsize) * x(length(x));
x = [pad_s0 x pad_s1];

for s = [(1+wsize):1:(length(x)-wsize)]
	dx_num = 0;
	dx_den = 0;
	for alpha = 1:wsize
		dx_num = dx_num + alpha*(x(s+alpha) - x(s-alpha));
		dx_den = dx_den + alpha^2;
	end
	dx_den = 2*dx_den;
	dx(s) = dx_num/dx_den;
end

dx = dx((1+wsize):(length(x)-wsize));
