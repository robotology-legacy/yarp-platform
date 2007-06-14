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

function lmLogCC(file_log, file_img, seq_num);

mtSimpleFig(1);
log = importdata(file_log, '/');
plot(log(:,1), log(:,2)/1000, 'r');
grid on;
%axis tight;
axis([0 length(log)-1 -256 256]);

fig_title = ['CC alignement correction - SEQ' num2str(seq_num)];
fig_title = sprintf('%s\nAvg.=%.3f', fig_title, mean(log(:,2))/1000);
fig_title = sprintf('%s/Std.=%.3f', fig_title, std(log(:,2))/1000);
title(fig_title);
xlabel('Word Number (WD)');
ylabel('Correction (kilo-samples)');

mtExport(1, file_img, '', 'png', 80);

