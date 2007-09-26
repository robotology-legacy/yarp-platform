function lmAG2Frames (ag_fn, ag_dir)

opt_cc = 0;
opt_us = 0;
opt_ag = 1;

opt_mode = 0;

%ag_fn  = '/mnt/contact_cache/exp_0001/aln_0002/wd_0000_ag.pos';
us_fn  = '/mnt/contact_cache/exp_0001/aln_0002/wd_0000_us.dv';
cc_fn  = '/mnt/contact_cache/exp_0001/aln_0002/wd_0000_cc.dv';
wav_fn = '/mnt/contact_cache/exp_0001/aln_0002/wd_0000_us.wav';
	
%system(sprintf('cp %s audio_dump.wav', wav_fn));

if(opt_us) 
	system('rm -f us/*');
	system(sprintf('mplayer %s -vo png -ao null', us_fn));
	system('mv *.png us/');
	system('cd us/; ctk_lmus');
end

if(opt_cc) 
	system('rm -f cc/*');
	system(sprintf('mplayer %s -vo png -ao null', cc_fn));
	system('mv *.png cc/');
	system('cd cc/; ctk_lmcc');
end


if(opt_ag) 
	system('rm -f ag/*');
	ag_data = importdata(ag_fn);
	ag_data = ag_data([1:200/25:length(ag_data)]', :);

	ag_size = size(ag_data);
	ag_dofs = ag_size(2);
	ag_len  = ag_size(1);
	
	if (opt_mode == 0)
		sensors = [1:3 6:7 8:9 11:12];
	else
		sensors = [1:3 6:7 8:9];
	end
	dofs    = [1:6:72];

	mtSimpleFig(1);
	frame = 1; %delay compensation!!!
	for sample = 1:ag_len
		
		clf;
		printf('Kine frame %d\n', frame);
	
		if (opt_mode == 0)
			set(gca,'Color','k', 'XColor','w', 'YColor','w', 'ZColor','w', 'FontSize', 20, 'LineWidth', 2)
			set(gcf, 'Color', [0 0 0])
			grid on;
			xlabel('Proximal-Distal [mm]');
			ylabel('Inferior-Superior [mm]');
			axis([-0.5 1.5 -1 1]*80);
		else
			set(gca,'Color','k', 'XColor','k', 'YColor','k', 'ZColor','k', 'FontSize', 20, 'LineWidth', 2)
			set(gcf, 'Color', [0 0 0])
			grid on;
			axis([-0.5 1.5 -1 1]*80);

			x_min =  min(min(ag_data(:, dofs(sensors) + 0)));
			x_max =  max(max(ag_data(:, dofs(sensors) + 0)));
			y_min =  min(min(ag_data(:, dofs(sensors) + 2)));
			y_max =  max(max(ag_data(:, dofs(sensors) + 2)));
			
			ax_min = min([x_min y_min]);
			ax_max = max([x_max y_max]);
			axis([ -ax_max -ax_min ax_min ax_max]);
		end
		hold on;
		for sns = sensors
			dof = dofs(sns);
			xc = -ag_data(sample, dof + 0);
			yc = +ag_data(sample, dof + 2);
			plot(xc, yc, 'ro', 'LineWidth', 5);
		end
		hold off;

		dof = dofs([1:12]);
		
		% Tongue sagittal
		hold on;
		x1 = -ag_data(sample, dof(1) + 0);
		y1 = +ag_data(sample, dof(1) + 2);
		x2 = -ag_data(sample, dof(2) + 0);
		y2 = +ag_data(sample, dof(2) + 2);
		x3 = -ag_data(sample, dof(3) + 0);
		y3 = +ag_data(sample, dof(3) + 2);
		plot([x1 x2], [y1 y2], 'y', 'LineWidth', 2);
		plot([x3 x2], [y3 y2], 'y', 'LineWidth', 2);
		hold off;

		% Teeth
		hold on;
		x6 = -ag_data(sample, dof(6) + 0);
		y6 = +ag_data(sample, dof(6) + 2);
		x7 = -ag_data(sample, dof(7) + 0);
		y7 = +ag_data(sample, dof(7) + 2);
		plot([x6 x7], [y6 y7], 'y', 'LineWidth', 2);
		hold off;
		
		% Lips
		hold on;
		x8 = -ag_data(sample, dof(8) + 0);
		y8 = +ag_data(sample, dof(8) + 2);
		x9 = -ag_data(sample, dof(9) + 0);
		y9 = +ag_data(sample, dof(9) + 2);
		plot([x8 x9], [y8 y9], 'y', 'LineWidth', 2);
		hold off;
	
		if (opt_mode == 0)
			% Glasses
			hold on;
			x11 = -ag_data(sample, dof(11) + 0);
			y11 = +ag_data(sample, dof(11) + 2);
			x12 = -ag_data(sample, dof(12) + 0);
			y12 = +ag_data(sample, dof(12) + 2);
			plot([x11 x12], [y11 y12], 'y', 'LineWidth', 2);
			hold off;
		end	

		hold on;
		for sns = sensors
			dof = dofs(sns);
			xc = -ag_data(sample, dof + 0);
			yc = +ag_data(sample, dof + 2);
			plot(xc, yc, 'k.', 'LineWidth', 2);
		end
		hold off;

		drawnow;
		pause(1/25);

		img = getframe(1);
		imwrite(img.cdata, sprintf('%s/%.8d.png', ag_dir, frame),'png')
		frame = frame + 1;
	end
end



