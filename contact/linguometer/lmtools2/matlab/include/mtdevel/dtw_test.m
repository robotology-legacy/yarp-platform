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

if(1)
	data = {};
	data{1} = load ('dist_0000/wd_0001.mat');
	data{2} = load ('dist_0001/wd_0001.mat');
	data{3} = load ('dist_0002/wd_0001.mat');
else
	data = {};
	data{1} = load ('../exp_0001/dist_0000/wd_0002.mat');
	data{2} = load ('../exp_0004/dist_0000/wd_0002.mat');
	data{3} = load ('../exp_0005/dist_0000/wd_0002.mat');
end

original = {};
original.rate = data{1}.misc.rate;
original.time{1} = data{1}.misc.time;
original.time{2} = data{2}.misc.time;
original.time{3} = data{3}.misc.time;

for type = 1
	if(type == 1)
		original.spc{1} = data{1}.US.spc;
		original.spc{2} = data{2}.US.spc;
		original.spc{3} = data{3}.US.spc;
		my_title = 'US-Speech';
	elseif(type == 2)
		original.pos{1} = data{1}.AG.pos(:, 3);
		original.pos{2} = data{2}.AG.pos(:, 3);
		original.pos{3} = data{3}.AG.pos(:, 3);
		my_title = 'AG-POS';
	elseif(type == 3)
		original.amp{1} = data{1}.AG.amp(:, 3);
		original.amp{2} = data{2}.AG.amp(:, 3);
		original.amp{3} = data{3}.AG.amp(:, 3);
		my_title = 'AG-AMP';
	end

	mtSimpleFig(type);
	for i = 1:3
		subplot(3, 2, 2*i - 1);
		plot(original.time{i}, original.spc{i}, 'r');
		grid on;
		axis tight;
		ylabel(sprintf('Signal %d', i));
		if(i == 1)
			title(sprintf('Original signals\n%s from %s', data{1}.misc.txt, my_title));
		elseif(i == 3)
			xlabel('Time [s]');
		end
	end

	warped = {};
	warped.rate   = original.rate;
	warped.spc{1} = original.spc{1};

	[warped.spc{2}, warped.var{2}] = mtWarp(warped.spc{1}, warped.rate, original.spc{2}, warped.rate);
	[warped.spc{3}, warped.var{3}] = mtWarp(warped.spc{1}, warped.rate, original.spc{3}, warped.rate);
	for i = 1:3
		warped.spc{i} = warped.spc{i}(2000:length(warped.spc{i})-2000);
	end
	warped.time   = mtTimeArray(warped.spc{1}, warped.rate);

	for i = 1:3
		subplot(3, 2, 2*i);
		plot(warped.time, warped.spc{i}, 'r');
		grid on;
		axis tight;
		ylabel(sprintf('Signal %d', i));
		if(i == 1)
			title(sprintf('Warped + zapped signals\n%s from %s', data{1}.misc.txt, my_title));
		elseif(i == 3)
			xlabel('Samples');
		end
	end
end
