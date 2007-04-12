function AG = lmProfile ()

printf ('[lmProfile] Loading profile:\n');
AG = [];
	
AG.name = 'AG500';

%---------------------
% Enviroment
%---------------------
if (isunix())
	AG.env.sep = '/';
else
	AG.env.sep = '\';
end

%---------------------
% Parameters
%---------------------
AG.pars.su = 'mm';		% space unit
%AG.pars.tu = 'ms';		% time unit
%AG.pars.tf = 1e-3; 	% time correction factor
AG.pars.tu = 's';		% time unit
AG.pars.tf = 1; 		% time correction factor
AG.pars.sr = 200;		% sampling frequency [HZ]
AG.pars.c = 0;			% AG center [mm]
AG.pars.r = 75;			% AG radius [mm]
%AG.pars.rp = 95;		% Plotting radius [mm]
%AG.pars.rmsMin = 0;		% target RMS
%AG.pars.rmsMax = 60;	% tolerated RMS
%AG.pars.isdMin = 12.5;  % minimum ISD (using cartridge)
						% between sensors of the same group [mm]
%AG.pars.isdMax = 37.5;  % minimum ISD (using cartridge) 
						% between sensors of the same group [mm]
AG.pars.nbc = 6;		% number of coils
AG.pars.nbd = 7;		% number of dimensions

%---------------------
% Dimensions
%---------------------
AG.d.x = 1;		% X
AG.d.y = 2;		% Y
AG.d.z = 3;		% Z
AG.d.p = 4;		% Phi
AG.d.t = 5; 	% Theta
AG.d.r = 6;		% RMS

%---------------------
% Labels: dimensions
%---------------------
AG.dlb {1} = ['X [' AG.pars.su ']'];
AG.dlb {2} = ['Y [' AG.pars.su ']'];
AG.dlb {3} = ['Z [' AG.pars.su ']'];
AG.dlb {4} = ['Phi [deg]'];
AG.dlb {5} = ['Theta [deg]'];
AG.dlb {6} = ['RMS'];

%---------------------
% Colors: dimensions
%         and clusters
%---------------------
AG.dcl (1) = 'b';
AG.dcl (2) = 'g';
AG.dcl (3) = 'r';
AG.dcl (4) = 'c';
AG.dcl (5) = 'm';
AG.dcl (6) = 'k';

%---------------------
% Styles: sensors
%---------------------
% Tongue, sagittal
AG.sst {1} = 'ob';
AG.sst {2} = 'og';
AG.sst {3} = 'or';
% Tongue, coronal
AG.sst {4} = '+c';
AG.sst {5} = '+m';
% Theeth
AG.sst {6} = 'xr';
AG.sst {7} = 'xk';
% Lips
AG.sst {8} = 'xm';
AG.sst {9} = 'xb';
% Glasses
AG.sst {10} = 'ob';
AG.sst {11} = 'or';
AG.sst {12} = 'ok';


%---------------------
% Style: points 
%---------------------
stp1 = {'ks', 'gs', 'rs', 'bs'};
stp2 = {'ko', 'go', 'ro', 'bo'};
stp3 = {'kd', 'gd', 'rd', 'bd'};
%stp1 = {'kv', 'gv', 'rv', 'bv'};
AG.style.points = [stp1 stp2 stp3];

%---------------------
% Style: lines
%---------------------
stt = {'k', 'g', 'r', 'b'};
AG.style.lines = [stt stt stt];


%---------------------
% Style: colors
%---------------------
stc = {'k', 'g', 'r', 'b'};
AG.style.colors = [stc stc stc];


%---------------------
% Style: colors 
%---------------------
stv = {'k', 'b', 'r', 'y', 'c', 'g'};
AG.style.colors = stv;

%---------------------
% Spam
%---------------------
printf ('  Profile Name:   %s\n',    AG.name);
printf ('  Time Unit:      %s\n',    AG.pars.tu);
printf ('  Time Factor:    %d\n',    AG.pars.tf);
printf ('  Sampling Rate:  %d Hz\n', AG.pars.sr);
printf ('  AG Center:      %d mm\n', AG.pars.c);
printf ('  AG Radius:      %d mm\n', AG.pars.r);
%printf ('  Target RMS:     %d\n',    AG.pars.rmsMin);
%printf ('  Tolerated RMS:  %d\n',    AG.pars.rmsMax);
printf ('  First sample:   %d s\n',  0);


