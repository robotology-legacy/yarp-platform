function lm_isd4hc_run(experiment)

opt_plot = 0;
AG = lmProfile();
%experiment = 1;
root = sprintf('/mnt/contact_data/Experiments/experiment_%.4d/AG/', experiment);


if (experiment == 1)
	%root = '/home/michele/ContactCache/exp_0001/root/AG/';
	data1 = lmLoadData(AG, [root 'Sweeps/rawpos/0001.pos']);
	data2 = lmLoadData(AG, [root 'Sweeps/rawpos/0002.pos']);
	data3 = lmLoadData(AG, [root 'Sweeps/rawpos/0003.pos']);
	data4 = lmLoadData(AG, [root 'Sweeps/rawpos/0004.pos']);
	data5 = lmLoadData(AG, [root 'Sweeps/rawpos/0005.pos']);
	data6 = lmLoadData(AG, [root 'Sweeps/rawpos/0007.pos']);
	data7 = lmLoadData(AG, [root 'Sweeps/rawpos/0008.pos']);
	data8 = lmLoadData(AG, [root 'Sweeps/rawpos/0009.pos']);
	data9 = lmLoadData(AG, [root 'Sweeps/rawpos/0010.pos']);
end

if (experiment == 2)
	%root = '/mnt/contact_data/Experiments/experiment_0002/AG/';
	data1 = lmLoadData(AG, [root 'Sweeps/rawpos/0001.pos']);
	data2 = lmLoadData(AG, [root 'Sweeps/rawpos/0002.pos']);
	data3 = lmLoadData(AG, [root 'Sweeps/rawpos/0005.pos']);
	data4 = lmLoadData(AG, [root 'Sweeps/rawpos/0006.pos']);
	data5 = lmLoadData(AG, [root 'Sweeps/rawpos/0008.pos']);
	data6 = lmLoadData(AG, [root 'Sweeps/rawpos/0009.pos']);
	data7 = lmLoadData(AG, [root 'Sweeps/rawpos/0012.pos']);
	data8 = lmLoadData(AG, [root 'Sweeps/rawpos/0013.pos']);
	data9 = lmLoadData(AG, [root 'Sweeps/rawpos/0014.pos']);
end

if (experiment == 3)
	%root = '/mnt/contact_data/Experiments/experiment_0002/AG/';
	data1 = lmLoadData(AG, [root 'Sweeps/rawpos/0001.pos']);
	data2 = lmLoadData(AG, [root 'Sweeps/rawpos/0002.pos']);
	data3 = lmLoadData(AG, [root 'Sweeps/rawpos/0003.pos']);
	data4 = lmLoadData(AG, [root 'Sweeps/rawpos/0004.pos']);
	data5 = lmLoadData(AG, [root 'Sweeps/rawpos/0005.pos']);
	data6 = lmLoadData(AG, [root 'Sweeps/rawpos/0006.pos']);
	data7 = lmLoadData(AG, [root 'Sweeps/rawpos/0007.pos']);
	data8 = lmLoadData(AG, [root 'Sweeps/rawpos/0008.pos']);
	data9 = lmLoadData(AG, [root 'Sweeps/rawpos/0009.pos']);
end

if (experiment == 4)
	%root = '/home/michele/ContactCache/exp_0004/root/AG/';
	data1 = lmLoadData(AG, [root 'Sweeps/rawpos/0001.pos']);
	data2 = lmLoadData(AG, [root 'Sweeps/rawpos/0002.pos']);
	data3 = lmLoadData(AG, [root 'Sweeps/rawpos/0003.pos']);
	data4 = lmLoadData(AG, [root 'Sweeps/rawpos/0007.pos']);
	data5 = lmLoadData(AG, [root 'Sweeps/rawpos/0008.pos']);
	data6 = lmLoadData(AG, [root 'Sweeps/rawpos/0009.pos']);
	data7 = lmLoadData(AG, [root 'Sweeps/rawpos/0010.pos']);
	data8 = lmLoadData(AG, [root 'Sweeps/rawpos/0011.pos']);
	data9 = lmLoadData(AG, [root 'Sweeps/rawpos/0012.pos']);
end

if (experiment == 5)
	%root = '/home/michele/ContactCache/exp_0005/root/AG/';
	data1 = lmLoadData(AG, [root 'Sweeps/rawpos/0001.pos']);
	data2 = lmLoadData(AG, [root 'Sweeps/rawpos/0002.pos']);
	data3 = lmLoadData(AG, [root 'Sweeps/rawpos/0003.pos']);
	data4 = lmLoadData(AG, [root 'Sweeps/rawpos/0004.pos']);
	data5 = lmLoadData(AG, [root 'Sweeps/rawpos/0005.pos']);
	data6 = lmLoadData(AG, [root 'Sweeps/rawpos/0006.pos']);
	data7 = lmLoadData(AG, [root 'Sweeps/rawpos/0007.pos']);
	data8 = lmLoadData(AG, [root 'Sweeps/rawpos/0008.pos']);
	data9 = lmLoadData(AG, [root 'Sweeps/rawpos/0009.pos']);
end

if (experiment == 6)
	%root = '/home/michele/ContactCache/exp_0006/root/AG/';
	data1 = lmLoadData(AG, [root 'Sweeps/rawpos/0001.pos']);
	data2 = lmLoadData(AG, [root 'Sweeps/rawpos/0002.pos']);
	data3 = lmLoadData(AG, [root 'Sweeps/rawpos/0003.pos']);
	data4 = lmLoadData(AG, [root 'Sweeps/rawpos/0004.pos']);
	data5 = lmLoadData(AG, [root 'Sweeps/rawpos/0005.pos']);
	data6 = lmLoadData(AG, [root 'Sweeps/rawpos/0006.pos']);
	data7 = lmLoadData(AG, [root 'Sweeps/rawpos/0007.pos']);
	data8 = lmLoadData(AG, [root 'Sweeps/rawpos/0008.pos']);
	data9 = lmLoadData(AG, [root 'Sweeps/rawpos/0009.pos']);
end

if (experiment == 8)
	%root = '/home/michele/ContactCache/exp_0008/root/AG/';
	data1 = lmLoadData(AG, [root 'Sweeps/rawpos/0001.pos']);
	data2 = lmLoadData(AG, [root 'Sweeps/rawpos/0002.pos']);
	data3 = lmLoadData(AG, [root 'Sweeps/rawpos/0003.pos']);
	data4 = lmLoadData(AG, [root 'Sweeps/rawpos/0004.pos']);
	data5 = lmLoadData(AG, [root 'Sweeps/rawpos/0005.pos']);
	data6 = lmLoadData(AG, [root 'Sweeps/rawpos/0006.pos']);
	data7 = lmLoadData(AG, [root 'Sweeps/rawpos/0007.pos']);
	data8 = lmLoadData(AG, [root 'Sweeps/rawpos/0008.pos']);
	data9 = lmLoadData(AG, [root 'Sweeps/rawpos/0009.pos']);
end

if (experiment == 9)
	%root = '/home/michele/ContactCache/exp_0009/root/AG/';
	data1 = lmLoadData(AG, [root 'Sweeps/rawpos/0001.pos']);
	data2 = lmLoadData(AG, [root 'Sweeps/rawpos/0002.pos']);
	data3 = lmLoadData(AG, [root 'Sweeps/rawpos/0003.pos']);
	data4 = lmLoadData(AG, [root 'Sweeps/rawpos/0004.pos']);
	data5 = lmLoadData(AG, [root 'Sweeps/rawpos/0005.pos']);
	data6 = lmLoadData(AG, [root 'Sweeps/rawpos/0006.pos']);
	data7 = lmLoadData(AG, [root 'Sweeps/rawpos/0007.pos']);
	data8 = lmLoadData(AG, [root 'Sweeps/rawpos/0008.pos']);
	data9 = lmLoadData(AG, [root 'Sweeps/rawpos/0009.pos']);
end

printf('Working on Sweep 1\n');
isd1_610  = lm_isd4hc_isd(AG, data1, 1, 6, 10, 1);
isd1_611  = lm_isd4hc_isd(AG, data1, 1, 6, 11, 1);
isd1_612  = lm_isd4hc_isd(AG, data1, 1, 6, 12, 1);
isd1_1112 = lm_isd4hc_isd(AG, data1, 1, 11, 12, 1);
isd1_1011 = lm_isd4hc_isd(AG, data1, 1, 10, 11, 1);
isd1_1012 = lm_isd4hc_isd(AG, data1, 1, 10, 12, 1);

printf('Working on Sweep 2\n');
isd2_610  = lm_isd4hc_isd(AG, data2, 1, 6, 10, 1);
isd2_611  = lm_isd4hc_isd(AG, data2, 1, 6, 11, 1);
isd2_612  = lm_isd4hc_isd(AG, data2, 1, 6, 12, 1);
isd2_1112 = lm_isd4hc_isd(AG, data2, 1, 11, 12, 1);
isd2_1011 = lm_isd4hc_isd(AG, data2, 1, 10, 11, 1);
isd2_1012 = lm_isd4hc_isd(AG, data2, 1, 10, 12, 1);

printf('Working on Sweep 3\n');
isd3_610  = lm_isd4hc_isd(AG, data3, 1, 6, 10, 1);
isd3_611  = lm_isd4hc_isd(AG, data3, 1, 6, 11, 1);
isd3_612  = lm_isd4hc_isd(AG, data3, 1, 6, 12, 1);
isd3_1112 = lm_isd4hc_isd(AG, data3, 1, 11, 12, 1);
isd3_1011 = lm_isd4hc_isd(AG, data3, 1, 10, 11, 1);
isd3_1012 = lm_isd4hc_isd(AG, data3, 1, 10, 12, 1);

printf('Working on Sweep 4\n');
isd4_610  = lm_isd4hc_isd(AG, data4, 1, 6, 10, 1);
isd4_611  = lm_isd4hc_isd(AG, data4, 1, 6, 11, 1);
isd4_612  = lm_isd4hc_isd(AG, data4, 1, 6, 12, 1);
isd4_1112 = lm_isd4hc_isd(AG, data4, 1, 11, 12, 1);
isd4_1011 = lm_isd4hc_isd(AG, data4, 1, 10, 11, 1);
isd4_1012 = lm_isd4hc_isd(AG, data4, 1, 10, 12, 1);

printf('Working on Sweep 5\n');
isd5_610  = lm_isd4hc_isd(AG, data5, 1, 6, 10, 1);
isd5_611  = lm_isd4hc_isd(AG, data5, 1, 6, 11, 1);
isd5_612  = lm_isd4hc_isd(AG, data5, 1, 6, 12, 1);
isd5_1112 = lm_isd4hc_isd(AG, data5, 1, 11, 12, 1);
isd5_1011 = lm_isd4hc_isd(AG, data5, 1, 10, 11, 1);
isd5_1012 = lm_isd4hc_isd(AG, data5, 1, 10, 12, 1);

printf('Working on Sweep 6\n');
isd6_610  = lm_isd4hc_isd(AG, data6, 1, 6, 10, 1);
isd6_611  = lm_isd4hc_isd(AG, data6, 1, 6, 11, 1);
isd6_612  = lm_isd4hc_isd(AG, data6, 1, 6, 12, 1);
isd6_1112 = lm_isd4hc_isd(AG, data6, 1, 11, 12, 1);
isd6_1011 = lm_isd4hc_isd(AG, data6, 1, 10, 11, 1);
isd6_1012 = lm_isd4hc_isd(AG, data6, 1, 10, 12, 1);

printf('Working on Sweep 7\n');
isd7_610  = lm_isd4hc_isd(AG, data7, 1, 6, 10, 1);
isd7_611  = lm_isd4hc_isd(AG, data7, 1, 6, 11, 1);
isd7_612  = lm_isd4hc_isd(AG, data7, 1, 6, 12, 1);
isd7_1112 = lm_isd4hc_isd(AG, data7, 1, 11, 12, 1);
isd7_1011 = lm_isd4hc_isd(AG, data7, 1, 10, 11, 1);
isd7_1012 = lm_isd4hc_isd(AG, data7, 1, 10, 12, 1);

printf('Working on Sweep 8\n');
isd8_610  = lm_isd4hc_isd(AG, data8, 1, 6, 10, 1);
isd8_611  = lm_isd4hc_isd(AG, data8, 1, 6, 11, 1);
isd8_612  = lm_isd4hc_isd(AG, data8, 1, 6, 12, 1);
isd8_1112 = lm_isd4hc_isd(AG, data8, 1, 11, 12, 1);
isd8_1011 = lm_isd4hc_isd(AG, data8, 1, 10, 11, 1);
isd8_1012 = lm_isd4hc_isd(AG, data8, 1, 10, 12, 1);

printf('Working on Sweep 9\n');
isd9_610  = lm_isd4hc_isd(AG, data9, 1, 6, 10, 1);
isd9_611  = lm_isd4hc_isd(AG, data9, 1, 6, 11, 1);
isd9_612  = lm_isd4hc_isd(AG, data9, 1, 6, 12, 1);
isd9_1112 = lm_isd4hc_isd(AG, data9, 1, 11, 12, 1);
isd9_1011 = lm_isd4hc_isd(AG, data9, 1, 10, 11, 1);
isd9_1012 = lm_isd4hc_isd(AG, data9, 1, 10, 12, 1);


printf('isd1 611: %.2f %.2f\n', mean(isd1_611), std(isd1_611));
printf('isd2 611: %.2f %.2f\n', mean(isd2_611), std(isd2_611));
printf('isd3 611: %.2f %.2f\n', mean(isd3_611), std(isd3_611));
printf('isd4 611: %.2f %.2f\n', mean(isd4_611), std(isd4_611));
printf('isd5 611: %.2f %.2f\n', mean(isd5_611), std(isd5_611));
printf('isd6 611: %.2f %.2f\n', mean(isd6_611), std(isd6_611));
printf('isd7 611: %.2f %.2f\n', mean(isd7_611), std(isd7_611));
printf('isd8 611: %.2f %.2f\n', mean(isd8_611), std(isd8_611));
printf('isd9 611: %.2f %.2f\n\n', mean(isd9_611), std(isd9_611));

printf('isd1 612: %.2f %.2f\n', mean(isd1_612), std(isd1_612));
printf('isd2 612: %.2f %.2f\n', mean(isd2_612), std(isd2_612));
printf('isd3 612: %.2f %.2f\n', mean(isd3_612), std(isd3_612));
printf('isd4 612: %.2f %.2f\n', mean(isd4_612), std(isd4_612));
printf('isd5 612: %.2f %.2f\n', mean(isd5_612), std(isd5_612));
printf('isd6 612: %.2f %.2f\n', mean(isd6_612), std(isd6_612));
printf('isd7 612: %.2f %.2f\n', mean(isd7_612), std(isd7_612));
printf('isd8 612: %.2f %.2f\n', mean(isd8_612), std(isd8_612));
printf('isd9 612: %.2f %.2f\n\n', mean(isd9_612), std(isd9_612));

printf('isd1 1112: %.2f %.2f\n', mean(isd1_1112), std(isd1_1112));
printf('isd2 1112: %.2f %.2f\n', mean(isd2_1112), std(isd2_1112));
printf('isd3 1112: %.2f %.2f\n', mean(isd3_1112), std(isd3_1112));
printf('isd4 1112: %.2f %.2f\n', mean(isd4_1112), std(isd4_1112));
printf('isd5 1112: %.2f %.2f\n', mean(isd5_1112), std(isd5_1112));
printf('isd6 1112: %.2f %.2f\n', mean(isd6_1112), std(isd6_1112));
printf('isd7 1112: %.2f %.2f\n', mean(isd7_1112), std(isd7_1112));
printf('isd8 1112: %.2f %.2f\n', mean(isd8_1112), std(isd8_1112));
printf('isd9 1112: %.2f %.2f\n', mean(isd9_1112), std(isd9_1112));

format = '  & %d & %.2f & %.2f & %.2f & %.2f & %.2f & %.2f & %.2f & %.2f & %.2f & %.2f & %.2f & %.2f\\\\\n';
ofile = sprintf('logisd_%.4d.txt',experiment);
fid = fopen(ofile,'wt');
fprintf(fid, format, 1, mean(isd1_610), std(isd1_610), mean(isd1_611), std(isd1_611), mean(isd1_612), std(isd1_612), mean(isd1_1112), std(isd1_1112), mean(isd1_1011), std(isd1_1011), mean(isd1_1012), std(isd1_1012));
fprintf(fid, format, 2, mean(isd2_610), std(isd2_610), mean(isd2_611), std(isd2_611), mean(isd2_612), std(isd2_612), mean(isd2_1112), std(isd2_1112), mean(isd2_1011), std(isd2_1011), mean(isd2_1012), std(isd2_1012));
fprintf(fid, format, 3, mean(isd3_610), std(isd3_610), mean(isd3_611), std(isd3_611), mean(isd3_612), std(isd3_612), mean(isd3_1112), std(isd3_1112), mean(isd3_1011), std(isd3_1011), mean(isd3_1012), std(isd3_1012));
fprintf(fid, format, 4, mean(isd4_610), std(isd4_610), mean(isd4_611), std(isd4_611), mean(isd4_612), std(isd4_612), mean(isd4_1112), std(isd4_1112), mean(isd4_1011), std(isd4_1011), mean(isd4_1012), std(isd4_1012));
fprintf(fid, format, 5, mean(isd5_610), std(isd5_610), mean(isd5_611), std(isd5_611), mean(isd5_612), std(isd5_612), mean(isd5_1112), std(isd5_1112), mean(isd5_1011), std(isd5_1011), mean(isd5_1012), std(isd5_1012));
fprintf(fid, format, 6, mean(isd6_610), std(isd6_610), mean(isd6_611), std(isd6_611), mean(isd6_612), std(isd6_612), mean(isd6_1112), std(isd6_1112), mean(isd6_1011), std(isd6_1011), mean(isd6_1012), std(isd6_1012));
fprintf(fid, format, 7, mean(isd7_610), std(isd7_610), mean(isd7_611), std(isd7_611), mean(isd7_612), std(isd7_612), mean(isd7_1112), std(isd7_1112), mean(isd7_1011), std(isd7_1011), mean(isd7_1012), std(isd7_1012));
fprintf(fid, format, 8, mean(isd8_610), std(isd8_610), mean(isd8_611), std(isd8_611), mean(isd8_612), std(isd8_612), mean(isd8_1112), std(isd8_1112), mean(isd8_1011), std(isd8_1011), mean(isd8_1012), std(isd8_1012));
fprintf(fid, format, 9, mean(isd9_610), std(isd9_610), mean(isd9_611), std(isd9_611), mean(isd9_612), std(isd9_612), mean(isd9_1112), std(isd9_1112), mean(isd9_1011), std(isd9_1011), mean(isd9_1012), std(isd9_1012));
fclose(fid);

mean_610 = [mean(isd1_610) mean(isd2_610) mean(isd3_610) mean(isd4_610) mean(isd5_610) mean(isd6_610) mean(isd7_610) mean(isd8_610) mean(isd9_610)];
mean_611 = [mean(isd1_611) mean(isd2_611) mean(isd3_611) mean(isd4_611) mean(isd5_611) mean(isd6_611) mean(isd7_611) mean(isd8_611) mean(isd9_611)];
mean_612 = [mean(isd1_612) mean(isd2_612) mean(isd3_612) mean(isd4_612) mean(isd5_612) mean(isd6_612) mean(isd7_612) mean(isd8_612) mean(isd9_612)];
mean_1112 = [mean(isd1_1112) mean(isd2_1112) mean(isd3_1112) mean(isd4_1112) mean(isd5_1112) mean(isd6_1112) mean(isd7_1112) mean(isd8_1112) mean(isd9_1112)];
mean_1011 = [mean(isd1_1011) mean(isd2_1011) mean(isd3_1011) mean(isd4_1011) mean(isd5_1011) mean(isd6_1011) mean(isd7_1011) mean(isd8_1011) mean(isd9_1011)];
mean_1012 = [mean(isd1_1012) mean(isd2_1012) mean(isd3_1012) mean(isd4_1012) mean(isd5_1012) mean(isd6_1012) mean(isd7_1012) mean(isd8_1012) mean(isd9_1012)];

std_610 = [std(isd1_610) std(isd2_610) std(isd3_610) std(isd4_610) std(isd5_610) std(isd6_610) std(isd7_610) std(isd8_610) std(isd9_610)];
std_611 = [std(isd1_611) std(isd2_611) std(isd3_611) std(isd4_611) std(isd5_611) std(isd6_611) std(isd7_611) std(isd8_611) std(isd9_611)];
std_612 = [std(isd1_612) std(isd2_612) std(isd3_612) std(isd4_612) std(isd5_612) std(isd6_612) std(isd7_612) std(isd8_612) std(isd9_612)];
std_1112 = [std(isd1_1112) std(isd2_1112) std(isd3_1112) std(isd4_1112) std(isd5_1112) std(isd6_1112) std(isd7_1112) std(isd8_1112) std(isd9_1112)];
std_1011 = [std(isd1_1011) std(isd2_1011) std(isd3_1011) std(isd4_1011) std(isd5_1011) std(isd6_1011) std(isd7_1011) std(isd8_1011) std(isd9_1011)];
std_1012 = [std(isd1_1012) std(isd2_1012) std(isd3_1012) std(isd4_1012) std(isd5_1012) std(isd6_1012) std(isd7_1012) std(isd8_1012) std(isd9_1012)];

if(0)
	xdom = [0:1:8];
	mtSimpleFig(1);
	hold on;
	errorbar(xdom, mean_1011, std_1011, 'r', 'Linewidth', 1);
	errorbar(xdom, mean_1012, std_1012, 'b', 'Linewidth', 1);
	errorbar(xdom, mean_1112, std_1112, 'k', 'Linewidth', 1);
	errorbar(xdom, mean_611, std_611, 'r--', 'Linewidth', 1);
	errorbar(xdom, mean_612, std_612, 'b--', 'Linewidth', 1);
	errorbar(xdom, mean_610, std_610, 'k--', 'Linewidth', 1);
	hold off;
	grid on;
	axis([-0.5 8.5 30 110]);
	the_title = sprintf('ISDs values - Experiment %d', experiment);
	title(the_title);
	xlabel('Sequence');
	ylabel('ISD [mm]');
	legend('ISD 10-11', 'ISD 10-12', 'ISD 11-12', 'ISD 6-11', 'ISD 6-12', 'ISD 6-10');
else	
	mtSimpleFig(experiment);
	subplot(1, 2, 1);
	xdom = [0:1:8];
	hold on;
	errorbar(xdom, mean_1011, std_1011, 'r', 'Linewidth', 1);
	errorbar(xdom, mean_1012, std_1012, 'b', 'Linewidth', 1);
	errorbar(xdom, mean_1112, std_1112, 'k', 'Linewidth', 1);
	hold off;
	grid on;
	axis([-0.5 8.5 30 110]);
	the_title = sprintf('ISDs values - Glasses\nExperiment %d', experiment);
	title(the_title);
	xlabel('Sequence');
	ylabel('ISD [mm]');
	legend('ISD 10-11', 'ISD 10-12', 'ISD 11-12');

	subplot(1, 2, 2);
	xdom = [0:1:8];
	hold on;
	errorbar(xdom, mean_611, std_611, 'r', 'Linewidth', 1);
	errorbar(xdom, mean_612, std_612, 'b', 'Linewidth', 1);
	errorbar(xdom, mean_610, std_610, 'k', 'Linewidth', 1);
	hold off;
	grid on;
	axis([-0.5 8.5 30 110]);
	the_title = sprintf('ISDs values - Glasses-Teeth\nExperiment %d', experiment);
	title(the_title);
	xlabel('Sequence');
	ylabel('ISD [mm]');
	legend('ISD 6-11', 'ISD 6-12', 'ISD 6-10');
    
end
