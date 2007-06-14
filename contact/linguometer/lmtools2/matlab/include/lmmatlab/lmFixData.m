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

function data_out = lmFixData (AG, data)
% Info:  creates a data structure the way I like
% Usage: data_out = lmFixData (AG, data)
% Where: AG        profile
%        data      input data
%        data_out  output data

%dsamples = length (data);
printf ('[lmFixData] Fixing Data:');

dsize = size (data);
dsamples = dsize (1);
printf (' Size %dx%dx%d, ', dsize(1), dsize(2) , dsize(3));

data_out = [];
if (dsize (2) == AG.pars.nbc)
	data_out.amp = data;
	data_out.pos = [];
	printf ('Type is AMP\n');
elseif (dsize (2) == AG.pars.nbd)
	data_out.amp = [];
	data_out.pos = data;
	printf ('Type is POS\n');
end


%data_out.data = data;
data_out.samples = dsamples;
data_out.tu = AG.pars.tu;
data_out.t0 = 0;
data_out.t1 = lmS2T(AG, dsamples - 1);
data_out.t = [data_out.t0:1/AG.pars.sr/AG.pars.tf:data_out.t1];
