function plot_pos (AG, data, sweep, bfig)

data_max = max(max(max(data.pos)));
data_min = min(min(min(data.pos)));

for sensor = 1:12
	mtSimpleFig(bfig + sensor);
	plot(data.t, data.pos(:, 1, sensor), AG.dcl(1)); hold on;
	plot(data.t, data.pos(:, 2, sensor), AG.dcl(2));
	plot(data.t, data.pos(:, 3, sensor), AG.dcl(3));
	plot(data.t, data.pos(:, 4, sensor), AG.dcl(4));
	plot(data.t, data.pos(:, 5, sensor), AG.dcl(5));
	%plot(data.t, data.pos(:, 6, sensor), AG.dcl(6));
	hold off;

	grid on;
	%axis([data.t0 data.t1 data_min data_max]);
	axis([data.t0 data.t1 -200 200]);
	
	string_title = sprintf('POS (All dims for each sensos)\nSweep %d, Sensor %d\n', sweep, sensor);
	title(string_title);

	ylabel(['Space [' AG.pars.su ']']);
	xlabel(['Time [' data.tu ']']);

	if (sensor == 1)
		%legend ('x', 'y', 'z', 'phi', 'theta', 'rms');
		legend ('x', 'y', 'z', 'phi', 'theta');
	end
end

%path_images = ['temp/images_' file_n];
%mtExportAll(path_images, 'png', '150');
