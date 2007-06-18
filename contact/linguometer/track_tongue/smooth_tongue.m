function smooth_tongue(fname)

if fname == "path_000000.txt"
  data=[1 1; 2 1; 3 1; 4 1];
else
  data=load(fname);
endif;

d = length(data);
dom = 1:1:d;

x = data(dom,1);
y = -data(dom,2);
[scs,p] = csaps(x,y,0.0001);

figure(1);
plot(x,y);
ax = [0 350 -300 50];
axis(ax,'square');
hold on;
[xx yy] = fnplt(scs);
plot(xx,yy,'rx');
hold off;
axis(ax,'square');
print([fname ".png"]);
drawnow

