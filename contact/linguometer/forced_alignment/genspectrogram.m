
wav = wavread("tmp/resampled.wav");

[h w] = size(wav);
if (w>1) 
  wav = wav(:,2);
endif;

[spec fs] = spectrify(wav,16000);

ph = load("tmp/rescaled.phseg");

[h w] = size(ph);
[specTop specLen] = size(spec);
scale = specLen/length(wav);
base = 0;
pre=1;
post=0;
for i=1:h
  row = ph(i,:);
  txt = char(row(3:5));
  silent = 1;
  if txt ~= "SIL"
    silent = 0;
    pre = 0;
  end;
#  if txt != "IGNORE"
    start = round(row(1)*scale);
    stop = round(row(2)*scale);
#    disp(txt);
    if start<specLen
      if ((!pre)||(!silent)) 
	if (!post)
	  spec(:,start+1) = 0;
	endif;
      endif;
    endif;
    if stop<specLen
      if (!silent) 
	spec(:,stop+1) = 0;
      endif;
    endif;
    #spec(base*10+(1:10),(start:stop)+1) = 0;
    base = 1-base;
#  endif;

  if txt == "SIL"
    if !pre
      post = 1;
    endif;
  endif;
endfor;

#imagesc(spec);
spec = spec + min(min(spec));
spec = round(254*spec/max(max(spec)));
imwrite("tmp/map.gif",spec);
save "tmp/scale.txt" scale;
