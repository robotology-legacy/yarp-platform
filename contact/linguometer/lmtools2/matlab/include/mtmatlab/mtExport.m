% Michele Tavella - michele@liralab.it
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
