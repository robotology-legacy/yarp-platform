load Hlp2030.mat
figure (1)
du = data_still.amp(:, 1, 1);

border  = ones(length(Hlp2030), 1);
border1 = border*du(1);
border2 = border*du(length(du));

du_t = [border1; du; border2];
df_t = filter (Hlp2030, 1, du_t, [], 1);

df = df_t(length(border1) + 1:length(border1) + length(du));

%df = filter (Hlp2030, 1, du, [], 1);

plot (du, 'k');
hold on;
plot (df, 'r');
hold off;
axis tight;
grid on;
