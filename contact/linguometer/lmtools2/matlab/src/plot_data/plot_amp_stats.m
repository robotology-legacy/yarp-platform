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

function plot_amp_stats (data)

mstd = zeros(6, 12);
for s = 1:12
	for d = 1:6
		array = data.amp(:, d, s);
		mstd (d, s) = std(array);
	end
end

mmin = zeros(6, 12);
for s = 1:12
	for d = 1:6
		array = data.amp(:, d, s);
		mmin (d, s) = min(array);
	end
end

mmax = zeros(6, 12);
for s = 1:12
	for d = 1:6
		array = data.amp(:, d, s);
		mmax (d, s) = max(array);
	end
end

mmean = zeros(6, 12);
for s = 1:12
	for d = 1:6
		array = data.amp(:, d, s);
		mmean (d, s) = mean(array);
	end
end

figure (1);
subplot (2, 3, 1:2);
bar(mstd');
xlabel ('Sensors');
ylabel ('std');
legend ('C1', 'C2', 'C3', 'C4', 'C5', 'C6');
string_title = sprintf('AMP\nStandard Deviation');
title (string_title);
grid on;

subplot (2, 3, 4:5);
bar(mmean');
xlabel ('Sensors');
ylabel ('mean');
string_title = sprintf('AMP\nMean');
title (string_title);
grid on;

subplot (2, 3, 3);
bar(mmax');
xlabel ('Sensors');
ylabel ('std');
string_title = sprintf('AMP\nMax');
title (string_title);
grid on;

subplot (2, 3, 6);
bar(mmin');
xlabel ('Sensors');
ylabel ('std');
string_title = sprintf('AMP\nMin');
title (string_title);
grid on;

