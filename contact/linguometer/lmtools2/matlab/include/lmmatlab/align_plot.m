mtSimpleFig(2);

if(0)
colors = ['rgbk' 'rk' 'gm' 'rk' 'gby']
hold on;
for i = 0:11
	plot3(data.AG.pos(:,i*6+1),data.AG.pos(:,i*6+2), data.AG.pos(:,i*6+3), colors(i+1))
end
hold off;
axis([-150 150 -150 150 -150 150]);
grid on;
end

mov=avifile('ex.avi');
colors = ['rgbk' 'rk' 'gm' 'rk' 'gby']
for panel = 1:4
	if(panel == 1)	
		clf
	end
	subplot(2, 2, panel);

	for s = 1:1920:length(data.AG.pos)
		hold on;
		% Sagittal tongue
		plot3(data.AG.pos(s,[1 7 13]),data.AG.pos(s,[2 8 14]), data.AG.pos(s,[3 9 15]), 'r')
		plot3(data.AG.pos(s,[1 7 13]),data.AG.pos(s,[2 8 14]), data.AG.pos(s,[3 9 15]), 'ko')
		
		% Coronal tongue
		plot3(data.AG.pos(s,[19 25]),data.AG.pos(s,[20 26]), data.AG.pos(s,[21 27]), 'b')
		plot3(data.AG.pos(s,[19 25]),data.AG.pos(s,[20 26]), data.AG.pos(s,[21 27]), 'ko')
		
		% Teeth
		plot3(data.AG.pos(s,[31 37]),data.AG.pos(s,[32 38]), data.AG.pos(s,[33 39]), 'b')
		plot3(data.AG.pos(s,[31 37]),data.AG.pos(s,[32 38]), data.AG.pos(s,[33 39]), 'ko')
		
		% Lips
		plot3(data.AG.pos(s,[43 49]),data.AG.pos(s,[44 50]), data.AG.pos(s,[45 51]), 'r')
		plot3(data.AG.pos(s,[43 49]),data.AG.pos(s,[44 50]), data.AG.pos(s,[45 51]), 'ko')
		
		% Head
		plot3(data.AG.pos(s,[55 61 67]),data.AG.pos(s,[56 62 68]), data.AG.pos(s,[57 63 69]), 'r')
		plot3(data.AG.pos(s,[55 61 67]),data.AG.pos(s,[56 62 68]), data.AG.pos(s,[57 63 69]), 'ko')

		hold off;
		axis([-1 1 -1 1 -1 1]*100);
		grid on;
		% YZ
		%view(90,0)
		% XY
		%view(0,0)
		% ZY
		view(0,90)
		pause2(1/25);
		F = getframe(gca);
		mov = addframe(mov,F);
	end
end
mov = close(mov);
