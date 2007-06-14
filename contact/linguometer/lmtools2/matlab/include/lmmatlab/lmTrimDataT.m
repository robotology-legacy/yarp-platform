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

function dataOut = lmTrimDataT(AG, dataIn, t0, t1);
% Info:  trims dataIn and fixes dataOut
% Usage: dataOut = mFixData (AG, dataIn)
% Where: AG        profile
%        dataIn    input data
%        t0        from time t0...
%        t1        ... to time t1, according 
%                  to AG.pars.tu time unit

s0 = lmT2S(AG, t0);
s1 = lmT2S(AG, t1);

printf ('[lmTrimDataT] Trimming data: %d to %d %s\n', t0, t1, AG.pars.tu);
dataOut = lmTrimDataS(AG, dataIn, s0, s1);
