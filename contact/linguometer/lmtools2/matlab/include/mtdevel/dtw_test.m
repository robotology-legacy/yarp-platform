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

if(0)
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

for type = 3
	if(type == 1)
		original.sig{1} = data{1}.US.spc;
		original.sig{2} = data{2}.US.spc;
		original.sig{3} = data{3}.US.spc;
		my_title = 'US-Speech';
	elseif(type == 2)
		original.sig{1} = data{1}.AG.pos(:, 3);
		original.sig{2} = data{2}.AG.pos(:, 3);
		original.sig{3} = data{3}.AG.pos(:, 3);
		my_title = 'AG-POS';
	elseif(type == 3)
		original.sig{1} = data{1}.AG.amp(:, 3);
		original.sig{2} = data{2}.AG.amp(:, 3);
		original.sig{3} = data{3}.AG.amp(:, 3);
		my_title = 'AG-AMP';
		
		filter_fast = ones(round(48000*0.05),1);
		original.sig{1} = filter2(filter_fast, original.sig{1}, 'same');
		original.sig{2} = filter2(filter_fast, original.sig{2}, 'same');
		original.sig{3} = filter2(filter_fast, original.sig{3}, 'same');
	end

	mtSimpleFig(type);
	for i = 1:3
		subplot(3, 2, 2*i - 1);
		plot(original.time{i}, original.sig{i}, 'r');
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
	warped.sig{1} = original.sig{1};
	warped.sig{2} = mtWarp (warped.sig{1}, warped.rate, original.sig{2}, warped.rate);
	warped.sig{3} = mtWarp (warped.sig{1}, warped.rate, original.sig{3}, warped.rate);
	for i = 1:3
		warped.sig{i} = warped.sig{i}(2000:length(warped.sig{i})-2000);
	end
	warped.time   = mtTimeArray(warped.sig{1}, warped.rate);

	for i = 1:3
		subplot(3, 2, 2*i);
		plot(warped.time, warped.sig{i}, 'r');
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
