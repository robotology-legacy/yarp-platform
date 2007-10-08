function v=soundspeed(t,m,g)
%SOUNDSPEED gives the speed of sound in air as a function of temperature V=(T,M,G)
%
%  Inputs:  T        air temperature in Celsius  [20 deg C]
%           M        average molecular weight of air [28.95 gm/mol]
%           G        adiabatic constant for air [1.4]
%
% Outputs:  V        is the speed of sound in m/s

%	   Copyright (C) Mike Brookes 2006
%      Version: $Id: soundspeed.m,v 1.1 2007-10-08 10:30:56 micheletavella Exp $
%
%   VOICEBOX is a MATLAB toolbox for speech processing.
%   Home page: http://www.ee.ic.ac.uk/hp/staff/dmb/voicebox/voicebox.html
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   This program is free software; you can redistribute it and/or modify
%   it under the terms of the GNU General Public License as published by
%   the Free Software Foundation; either version 2 of the License, or
%   (at your option) any later version.
%
%   This program is distributed in the hope that it will be useful,
%   but WITHOUT ANY WARRANTY; without even the implied warranty of
%   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%   GNU General Public License for more details.
%
%   You can obtain a copy of the GNU General Public License from
%   ftp://prep.ai.mit.edu/pub/gnu/COPYING-2.0 or by writing to
%   Free Software Foundation, Inc.,675 Mass Ave, Cambridge, MA 02139, USA.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if nargin<3
    g=1.4;
    if nargin<2
        m=28.95;
        if nargin<1
            t=20;
        end
    end
end
r=8.3144;  % J/(mol K) universal gas constant
v=sqrt(g*r*(t+273.15)*1000/m);
