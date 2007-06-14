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

function mtExport (fig, filename, path, driver, dpi)

if (strcmp (filename, 'auto'))
	filename = sprintf ('%.6d', fig);
	filename = strcat ('image_', filename);
	
	if (strcmp (path, 'auto'))
		filename = strcat ('images/', filename);
	else
		if (exist (path, 'dir') == false)
			printf ('[mExport] Creating folder: %s\n', path);
			mkdir (path)
		end
		filename = strcat ([path '/'], filename);
	end
end
%driver = 'ps'       % PostScript for black and white printers
%driver = 'psc'      % PostScript for color printers
%driver = 'ps2'      % Level 2 PostScript for black and white printers
%driver = 'psc2'     % Level 2 PostScript for color printers

%driver = 'eps'      % Encapsulated PostScript
%driver = 'epsc'     % Encapsulated Color PostScript
%driver = 'eps2':     % Encapsulated Level 2 PostScript
%driver = 'epsc2';    % Encapsulated Level 2 Color PostScript
%driver = 'ppm';    % Encapsulated Level 2 Color PostScript

drivercmd = ['-d' driver];
if (strcmp (driver, 'psc2'))
	extension = '.ps';
else
	extension = ['.' driver];
end
file 	  = [filename extension];

printf ('[mExport] Exporting: %s\n', file);
print (drivercmd, ['-f' num2str(fig)], ['-r' int2str(dpi)], ['' file]);
