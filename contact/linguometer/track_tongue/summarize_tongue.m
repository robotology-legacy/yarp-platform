
global all_curve;


%toggle_octplot
clf;
N = length(all_curve);
for i=1:N
  i
  v = all_curve{i};
  x = v{1};
  y = v{2};
  hold on;
  h = plot(x,y);
  set(h,"Color",[(N-i)/N, (i-1)/N, 2*abs(i-N/2)]);
%  set(h,"linewidth",i);
  hold off;
endfor;
axis([0 300 -250 -50]);
print("summary.png");
%toggle_octplot

