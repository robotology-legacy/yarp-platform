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

load Hlp2030.mat
figure (1)
du = data_still.amp(:, 1, 1);

border  = ones(length(Hlp2030), 1);
border1 = border*du(1);
border2 = border*du(length(du));

du_t = [border1; du; border2];
df_t = filter (Hlp2030, 1, du_t, [], 1);

df = df_t(length(border1) + 1:length(border1) + length(du));

%df = filter (Hlp2030, 1, du, [], 1);

plot (du, 'k');
hold on;
plot (df, 'r');
hold off;
axis tight;
grid on;
