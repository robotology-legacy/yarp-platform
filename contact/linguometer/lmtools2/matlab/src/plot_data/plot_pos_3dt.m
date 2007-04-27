data_max = max(max(max(data.pos)));
data_min = min(min(min(data.pos)));

mtSimpleFig (100);
frame = 1;
for sample = 1:8:length(data.pos)
	clf (100)
	hold on;
	for sensor = [1 2 3 4 5 6 7 8 9 10 12]
		X = data.pos(sample, 1, sensor);		
		Y = data.pos(sample, 2, sensor);		
		Z = data.pos(sample, 3, sensor);
		
		string_style = cell2mat(AG.sst(sensor));
		plot3(X, Y, Z, string_style, 'LineWidth', 2);
	end


	hold off;
	axis([-200 200 -200 200 -200 200]);
	%axis([-50 50 -50 50 -50 50]);
	grid on;
	xlabel('x');
	ylabel('y');
	zlabel('z');
	%view(0,90);
	drawnow
	%F(frame) = getframe(gcf);
	frame = frame + 1;
	if (sample >= 100)
		%break;
	end
	pause (1/25);
end

break;
fig=figure;
set(fig,'DoubleBuffer','on');
set(gca,'xlim',[-80 80],'ylim',[-80 80],...
    'NextPlot','replace','Visible','off')
mov = avifile('example.avi')
x = -pi:.1:pi;
radius = 0:length(x);
for k=1:length(x)
 h = patch(sin(x)*radius(k),cos(x)*radius(k),...
    [abs(cos(x(k))) 0 0]);
 set(h,'EraseMode','xor');
 F = getframe(gca);
 mov = addframe(mov,F);
end
mov = close(mov);
