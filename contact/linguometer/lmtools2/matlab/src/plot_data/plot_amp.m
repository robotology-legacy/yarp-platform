function plot_pos (AG, data, sweep, bfig)

data_max = max(max(max(data.amp)));
data_min = min(min(min(data.amp)));

for sensor = 1:12
	mtSimpleFig(bfig + sensor);
	plot(data.t, data.amp(:, 1, sensor), 'b'); hold on;
	plot(data.t, data.amp(:, 2, sensor), 'g');
	plot(data.t, data.amp(:, 3, sensor), 'r');
	plot(data.t, data.amp(:, 4, sensor), 'c');
	plot(data.t, data.amp(:, 5, sensor), 'm');
	plot(data.t, data.amp(:, 6, sensor), 'k'); hold off;


	grid on;
	axis([data.t0 data.t1 data_min data_max]);
	
	string_title = sprintf('AMP (All coils for each sensor)\nSweep %d, Sensor %d\n', sweep, sensor);
	title(string_title);

	ylabel('Amplitudes');
	xlabel(['Time [' data.tu ']']);
	
	if (sensor == 1)
		legend ('C1', 'C2', 'C3', 'C4', 'C5', 'C6');
	end
end

%path_images = ['temp/images_' file_n];
%mtExportAll(path_images, 'png', '150');
