function wav = align_zap(wav, rate, do_invert)
if (do_invert == 0)
    [ignore peak] = max(wav);
    wav(1:(peak+round(rate*0.1)),:) = 0;
    [ignore peak] = min(wav);
    wav((peak-round(rate*0.1)):length(wav),:) = 0; 
else
    [ignore peak] = max(wav);
    wav((peak-round(rate*0.1)):length(wav),:) = 0;
    [ignore peak] = min(wav);
    wav(1:(peak+round(rate*0.1)),:) = 0;
end
