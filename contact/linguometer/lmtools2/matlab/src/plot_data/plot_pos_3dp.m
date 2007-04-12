%data_max = max(max(max(data.pos)));
%data_min = min(min(min(data.pos)));

export_movie = 0;
fig   = 110;
frame = 1;
if (export_movie)
	mov = avifile('example.avi', 'fps', 25);
end
% -----------------------------------------------------------------------------%
sensors 				= 1:12;
sensors_tongue_sagittal = 1:3;
sensors_tongue_coronal  = 4:5;
sensors_teeth  			= 6:7;
sensors_lips  			= 8:9;
sensors_glasses 		= 10:12;
% -----------------------------------------------------------------------------%
mtSimpleFig (fig);
set(fig,'DoubleBuffer','on');
set(fig, 'NextPlot','replace');
for sample = 1:8:length(data.pos)
    % -------------------------------------------------------------------------%
	clf (fig)
    % -------------------------------------------------------------------------%
	X = data.pos(sample, 1, sensors);		
	Y = data.pos(sample, 2, sensors);		
	Z = data.pos(sample, 3, sensors);
    % -------------------------------------------------------------------------%
	% Tounge sagittal
	x1 = [X(1) X(2)];
	z1 = [Z(1) Z(2)];
	x2 = [X(2) X(3)];
	z2 = [Z(2) Z(3)];
	hold on;
		line (x1, z1, 'Color', 'k');
		line (x2, z2, 'Color', 'k');
		for sensor = sensors_tongue_sagittal
			string_style = cell2mat(AG.sst(sensor));
			plot(X(sensor), Z(sensor),  string_style, 'LineWidth', 4);
		end
	hold off;
    % -------------------------------------------------------------------------%
	% Teeth
	x1 = [X(6) X(7)];
	z1 = [Z(6) Z(7)];
	hold on;
		line (x1, z1, 'Color', 'k');
		for sensor = sensors_teeth
			string_style = cell2mat(AG.sst(sensor));
			plot(X(sensor), Z(sensor),  string_style, 'LineWidth', 4);
		end
	hold off;
    % -------------------------------------------------------------------------%
	% Lips
	x1 = [X(8) X(9)];
	z1 = [Z(8) Z(9)];
	hold on;
		line (x1, z1, 'Color', 'k');
		for sensor = sensors_lips
			string_style = cell2mat(AG.sst(sensor));
			plot(X(sensor), Z(sensor),  string_style, 'LineWidth', 4);
		end
	hold off;
    % -------------------------------------------------------------------------%
	string_title = sprintf ('Sagittal view\n(Frame %.5d, 25fps)', frame);
	title (string_title);
	xlabel ('x [mm]');
	ylabel ('z [mm]');
	%axis([-100 100 -100 100]);
	axis([-80 20 -80 20]);
	grid on;
    % -------------------------------------------------------------------------%
	drawnow
	if (export_movie)
		F = getframe(fig);
		mov = addframe(mov, F);
		if (frame >= 1000)
			break;
		end
		frame = frame + 1;
		pause (1/100);
	end
	pause (1/25);
    % -------------------------------------------------------------------------%
end
printf('Total Frames: %d\n', frame);
if (export_movie)
	mov = close(mov);
end
