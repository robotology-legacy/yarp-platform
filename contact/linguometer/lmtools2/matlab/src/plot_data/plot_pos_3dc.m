function plot_pos_3dc (AG, data, sweep, sensors, bfig);


data_max = max(max(max(data.pos)));
data_min = min(min(min(data.pos)));

mtSimpleFig (bfig);
hold on;
for sensor = sensors
	X = data.pos(:, 1, sensor);		
	Y = data.pos(:, 2, sensor);		
	Z = data.pos(:, 3, sensor);
	
	string_style = cell2mat(AG.sst(sensor));
	plot3(X, Y, Z, string_style);
end
hold off;
axis([-200 200 -200 200 -200 200]);
grid on;
xlabel('x');
ylabel('y');
zlabel('z');

string_title = sprintf ('POS (cluster view)\nSweep %d', sweep);
title (string_title);

%path_images = ['temp/images_' file_n];
%mtExportAll(path_images, 'png', '150');
%view (0, 90)
view (-29, 18)
axis square 
