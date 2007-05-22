function lmLogCC(file_log, file_img, seq_num);

mtSimpleFig(1);
log = importdata(file_log, '/');
plot(log(:,1), log(:,2)/1000, 'r');
grid on;
%axis tight;
axis([0 length(log)-1 -256 256]);

fig_title = ['CC alignement correction - SEQ' num2str(seq_num)];
fig_title = sprintf('%s\nAvg.=%.3f', fig_title, mean(log(:,2))/1000);
fig_title = sprintf('%s/Std.=%.3f', fig_title, std(log(:,2))/1000);
title(fig_title);
xlabel('Word Number (WD)');
ylabel('Correction (kilo-samples)');

mtExport(1, file_img, '', 'png', 80);

