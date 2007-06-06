function [wav zap0 zap1] = align_zap(wav, rate, do_invert)
if (do_invert == 0)
    [ignore peak] = max(wav);
	zap0 = peak+round(rate*0.1);
    wav(1:zap0,:) = 0;
    [ignore peak] = min(wav);
	zap1 = peak-round(rate*0.1);
    wav(zap1:length(wav),:) = 0; 
else
    [ignore peak] = max(wav);
	zap0 = peak-round(rate*0.1);
    wav(zap0:length(wav),:) = 0;
    [ignore peak] = min(wav);
	zap1 = peak+round(rate*0.1);
    wav(1:zap1,:) = 0;
end
