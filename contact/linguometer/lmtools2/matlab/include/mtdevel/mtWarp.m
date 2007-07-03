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
% function sig2x = mtWarp (sig1, sr1, sig2, sr2)

function [sig2x, var] = mtWarp (sig1, sr1, sig2, sr2)

opt_plot = 0;

D1 = specgram(sig1,512,sr1,512,384);
D2 = specgram(sig2,512,sr2,512,384);

SM = simmx(abs(D1), abs(D2));

% Look at it:
if (opt_plot)
	subplot(121)
	imagesc(SM)
	colormap(1-gray)
end

[p,q,C] = dp(1-SM);
if (opt_plot)
	hold on; plot(q,p,'r'); hold off

	subplot(122)
	imagesc(C)
	hold on; plot(q,p,'r'); hold off
end
%C(size(C,1),size(C,2));

D2i1 = zeros(1, size(D1,2));
for i = 1:length(D2i1);
	D2i1(i) = q(min(find(p >= i))); 
end
D2x = pvsample(D2, D2i1-1, 128);
sig2x = istft(D2x, 512, 512, 128);

sig2x = resize(sig2x', length(sig1),1);

var.sig1 = sig1;
var.D1 = D1;
var.D2 = D2;
var.p  = p;
var.q  = q;
var.C  = C;
var.SM = SM;
