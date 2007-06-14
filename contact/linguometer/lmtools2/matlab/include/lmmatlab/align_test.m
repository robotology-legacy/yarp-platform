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

a = zeros(1, 50);
b = zeros(1, 50);

b(1, 10:20) = 1;
a(1, 30:40) = 1;

offset_b = align_lag(a, b)
b2 = lagmatrix(b, offset_b);

mtSimpleFig(100);
plot(a, 'r');
hold on;
plot(b, 'k');
plot(b2, 'g.');
hold off;
grid on;
axis tight;
legend('a', 'b');

b3 = align_crop(b2, offset_b);
