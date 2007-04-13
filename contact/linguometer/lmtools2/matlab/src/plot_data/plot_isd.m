function ISD = plot_isd (AG, data, sweep, sensor_a, sensor_b, bfig)

for sample = 1:data.samples
	pA = data.pos(sample, 1:3, sensor_a);
	pB = data.pos(sample, 1:3, sensor_b);
	ISD(sample) = mtED (pA, pB);
end

mtSimpleFig(bfig);
plot(data.t, ISD, 'r');
string_title = sprintf('ISD: sensors %d and %d\n', sensor_a, sensor_b);
title(string_title);
ylabel(['Space [' AG.pars.su ']']);
xlabel(['Time [' data.tu ']']);
grid on;
axis ([data.t0 data.t1 0 200]);
