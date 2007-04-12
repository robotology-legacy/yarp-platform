function mtExportAll (path, driver, dpi)

if (isempty (dpi) == true)
	dpi = 300;
end

if (isempty (driver) == true)
	driver = 'psc2';
end

if (isempty (path) == true)
	path = 'auto';
end

for fig = 1:20000
	if (ishandle (handle (fig)) ~= 0)
		mtExport (fig, 'auto', path, driver, dpi);
	end
end
