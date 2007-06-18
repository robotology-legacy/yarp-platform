function smooth_tongue(fname)

data=load(fname);

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

