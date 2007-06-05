a = zeros(1, 50);
b = zeros(1, 50);

b(1, 10:20) = 1;
a(1, 30:40) = 1;

offset_b = align_lag(a, b)
b2 = lagmatrix(b, offset_b);

mtSimpleFig(100);
plot(a, 'r');
hold on;
plot(b, 'k');
plot(b2, 'g.');
hold off;
grid on;
axis tight;
legend('a', 'b');

b3 = align_crop(b2, offset_b);
