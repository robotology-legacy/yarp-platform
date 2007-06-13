function sig2 = lmpkgCrop(sig1, offset)
if (offset > 0)	
	%sig2 = sig1(offset+1:length(sig1));
    sig2 = sig1;
    sig2(1:offset, :) = 0;
elseif (offset < 0)
	sig2 = sig1(1:length(sig1)+offset-1, :);
else
	sig2 = sig1;
end
