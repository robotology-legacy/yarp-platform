function lmpkgAlignProxy(wd0, wd1, seq0, seq1)

for seq = seq0:seq1
	for wd = wd0:wd1
		lmpkgAlign(wd, seq);
	end
end
