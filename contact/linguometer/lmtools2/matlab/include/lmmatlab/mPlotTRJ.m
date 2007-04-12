function mPlotTRJ (AG, data, ch, idx, use_limits, f, t);
% Usage: mPlotTRJ (AG, data, ch, idx, use_limits, f)
% Where: AG          profile
%        data        data structure 
%        chs         channels (1, 2, ..., 12)
%        idx         index/labels (x, y, z...)
%        use_limits  limits as reference for a
%                    correct acquisition
%        f           figure
%        t           title comment

printf ('[mPlotTRJ] Plotting Ch. %d (Figure %d)\n', ch, f);
plots = [];
plots.number  = length (idx);
plots.usetime = true;

rms_m   = mean (data.data (:, AG.idx.r, ch));
rms_min = min  (data.data (:, AG.idx.r, ch));
rms_max = max  (data.data (:, AG.idx.r, ch));

m_figure (f);

for li = 1:plots.number
	if (plots.number > 1)
		subplot (plots.number, 1, li);
	end

	%plot (data.time.domain, data.data (:, idx (li), ch), 'r');
	plot (data.time.domain, data.data (:, idx (li), ch), 'r');
	
	if (li < plots.number)
		ax = gca;
		set (ax, 'XTickLabel', []);
	end

	if (use_limits == true)
		if (idx (li) == AG.idx.r)
			axis ([data.time.t0 data.time.t1 AG.pars.rmsMin AG.pars.rmsMax]);
		elseif (idx (li) == AG.idx.p | idx (li) == AG.idx.t)
			axis tight;
		else
			axis ([data.time.t0 data.time.t1 -AG.pars.r AG.pars.r]);
		end
	else
		axis tight;
	end

	grid on;

	if (li == 1)
		if (isempty (t))
			title (sprintf ('TRJ: Channel %d\n[RMS Min/Mean/Max = %.1d/%.1d/%.1d]', ch, rms_min, rms_m, rms_max));
		else
			title (sprintf ('TRJ: Channel %d\n[RMS Min/Mean/Max = %.1d/%.1d/%.1d]\n%s', ch, rms_min, rms_m, rms_max, t));
		end
	elseif (li == plots.number)
		xlabel (sprintf ('time [%s]', AG.pars.tu));
	end
	

	ylabel (AG.labels (idx(li)));
end