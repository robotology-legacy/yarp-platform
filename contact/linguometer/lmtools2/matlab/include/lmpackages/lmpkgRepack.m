% Copyright (C) 2007 Michele Tavella <michele@liralab.it>
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

function retval=lmpkgRepack(matfile)

cmd_stdo = '1>/dev/null  2>/dev/null';

printf('[lmpkgRepack] Loading %s\n', matfile);
data = load(matfile);


%function test = lmpkgComputeSlowOffset (dataptr, datasr, rawsr, idxcol)
%retval=lmpkgComputeSlowOffset(data.aln.CC.fea, data.aln.CC.fea_rate, data.raw.CC.fea_rate, 1);
%return;

% Compute the 48 kHz offset back in original rates
raw.off.US_spc = lmpkgResampleOffset(data.off1.us, data.misc.rate, data.raw.US.spc_rate, 'US_spc');
raw.off.US_fea = lmpkgResampleOffset(data.off1.us, data.misc.rate, data.raw.US.fea_rate, 'US_fea');
raw.off.AG_spc = lmpkgResampleOffset(data.off1.ag, data.misc.rate, data.raw.AG.spc_rate, 'AG_spc');
raw.off.AG_amp = lmpkgResampleOffset(data.off1.ag, data.misc.rate, data.raw.AG.amp_rate, 'AG_amp');
raw.off.AG_pos = lmpkgResampleOffset(data.off1.ag, data.misc.rate, data.raw.AG.pos_rate, 'AG_pos');
raw.off.CC_spc = lmpkgResampleOffset(data.off1.cc, data.misc.rate, data.raw.CC.spc_rate, 'CC_spc');
raw.off.CC_fea = lmpkgResampleOffset(data.off1.cc, data.misc.rate, data.raw.CC.fea_rate, 'CC_fea');
if(data.misc.status.LG)
	raw.off.LG_spc = lmpkgResampleOffset(data.off1.lg, data.misc.rate, data.raw.LG.spc_rate, 'LG_spc');
	raw.off.LG_egg = lmpkgResampleOffset(data.off1.lg, data.misc.rate, data.raw.LG.egg_rate, 'LG_egg');
end

% Compute zap values at 25Hz (slowest signal)
% In this case, compute s0, that would be the first DV frame
zapt_s0 = data.misc.zap.s0 / data.misc.rate * data.raw.US.fea_rate;
zap_s0 = ceil(zapt_s0) * data.misc.rate / data.raw.US.fea_rate;

% Compute zap values at 25Hz (slowest signal)
% In this case, compute s1, that would be the last DV frame
zapt_s1 = data.misc.zap.s1 / data.misc.rate * data.raw.US.fea_rate;
zap_s1 = floor(zapt_s1) * data.misc.rate / data.raw.US.fea_rate - 1;

% It's about time to compute the zap values for the raw signals
raw.zap.US_spc.s0 = round(zap_s0 / data.misc.rate * data.raw.US.spc_rate);
raw.zap.US_fea.s0 = round(zap_s0 / data.misc.rate * data.raw.US.fea_rate); 
raw.zap.AG_spc.s0 = round(zap_s0 / data.misc.rate * data.raw.AG.spc_rate);  
raw.zap.AG_amp.s0 = round(zap_s0 / data.misc.rate * data.raw.AG.amp_rate);  
raw.zap.AG_pos.s0 = round(zap_s0 / data.misc.rate * data.raw.AG.pos_rate); 
raw.zap.CC_spc.s0 = round(zap_s0 / data.misc.rate * data.raw.CC.spc_rate);
raw.zap.CC_fea.s0 = round(zap_s0 / data.misc.rate * data.raw.CC.fea_rate);
if(data.misc.status.LG)
	raw.zap.LG_spc.s0 = round(zap_s0 / data.misc.rate * data.raw.LG.spc_rate);
	raw.zap.LG_egg.s0 = round(zap_s0 / data.misc.rate * data.raw.LG.egg_rate);
end

raw.zap.US_spc.s1 = round(zap_s1 / data.misc.rate * data.raw.US.spc_rate);
raw.zap.US_fea.s1 = round(zap_s1 / data.misc.rate * data.raw.US.fea_rate); 
raw.zap.AG_spc.s1 = round(zap_s1 / data.misc.rate * data.raw.AG.spc_rate);  
raw.zap.AG_amp.s1 = round(zap_s1 / data.misc.rate * data.raw.AG.amp_rate);  
raw.zap.AG_pos.s1 = round(zap_s1 / data.misc.rate * data.raw.AG.pos_rate); 
raw.zap.CC_spc.s1 = round(zap_s1 / data.misc.rate * data.raw.CC.spc_rate);
raw.zap.CC_fea.s1 = round(zap_s1 / data.misc.rate * data.raw.CC.fea_rate);
if(data.misc.status.LG)
	raw.zap.LG_spc.s1 = round(zap_s1 / data.misc.rate * data.raw.LG.spc_rate);
	raw.zap.LG_egg.s1 = round(zap_s1 / data.misc.rate * data.raw.LG.egg_rate);
end


if(0)
	v = [1:1:20]';
	off1 = +2;
	off2 = -2;
	u1r = lagmatrix(v, off1);
	u2r = lagmatrix(v, off2);
	u1 = [zeros(off1,1); v(1:length(v)-off1)];
	u2 = v(-off2+1:length(v));
end

% Lag compensation
printf('[lmpkgRepack] Performing lag correction\n');
raw.lagd.US.spc = lagmatrix(data.raw.US.spc, raw.off.US_spc);
raw.lagd.US.fea = lagmatrix(data.raw.US.fea, raw.off.US_fea);
raw.lagd.AG.spc = lagmatrix(data.raw.AG.spc, raw.off.AG_spc);
raw.lagd.AG.amp = lagmatrix(data.raw.AG.amp, raw.off.AG_amp);
raw.lagd.AG.pos = lagmatrix(data.raw.AG.pos, raw.off.AG_pos);
raw.lagd.CC.spc = lagmatrix(data.raw.CC.spc, raw.off.CC_spc);
raw.lagd.CC.fea = lagmatrix(data.raw.CC.fea, raw.off.CC_fea);
if(data.misc.status.LG)
	raw.lagd.LG.spc = lagmatrix(data.raw.LG.spc, raw.off.LG_spc);
	raw.lagd.LG.egg = lagmatrix(data.raw.LG.egg, raw.off.LG_egg);
end


% Cropping signals
printf('[lmpkgRepack] Cropping dataset\n');
raw.cropd.US.spc = lmpkgCrop(raw.lagd.US.spc, raw.off.US_spc);
raw.cropd.US.fea = lmpkgCrop(raw.lagd.US.fea, raw.off.US_fea);
raw.cropd.AG.spc = lmpkgCrop(raw.lagd.AG.spc, raw.off.AG_spc);
raw.cropd.AG.amp = lmpkgCrop(raw.lagd.AG.amp, raw.off.AG_amp);
raw.cropd.AG.pos = lmpkgCrop(raw.lagd.AG.pos, raw.off.AG_pos);
raw.cropd.CC.spc = lmpkgCrop(raw.lagd.CC.spc, raw.off.CC_spc);
raw.cropd.CC.fea = lmpkgCrop(raw.lagd.CC.fea, raw.off.CC_fea);
if(data.misc.status.LG)
	raw.cropd.LG.spc = lmpkgCrop(raw.lagd.LG.spc, raw.off.LG_spc);
	raw.cropd.LG.egg = lmpkgCrop(raw.lagd.LG.egg, raw.off.LG_egg);
end

 % Zapping clicks
printf('[lmpkgRepack] Zapping dataset\n');
raw.zapd.US.spc = raw.cropd.US.spc(raw.zap.US_spc.s0:raw.zap.US_spc.s1, :);
raw.zapd.US.fea = raw.cropd.US.fea(raw.zap.US_fea.s0:raw.zap.US_fea.s1, :);
raw.zapd.AG.spc = raw.cropd.AG.spc(raw.zap.AG_spc.s0:raw.zap.AG_spc.s1, :);
raw.zapd.AG.amp = raw.cropd.AG.amp(raw.zap.AG_amp.s0:raw.zap.AG_amp.s1, :);
raw.zapd.AG.pos = raw.cropd.AG.pos(raw.zap.AG_pos.s0:raw.zap.AG_pos.s1, :);
raw.zapd.CC.spc = raw.cropd.CC.spc(raw.zap.CC_spc.s0:raw.zap.CC_spc.s1, :);
raw.zapd.CC.fea = raw.cropd.CC.fea(raw.zap.CC_fea.s0:raw.zap.CC_fea.s1, :);
if(data.misc.status.LG)
	raw.zapd.LG.spc = raw.cropd.LG.spc(raw.zap.LG_spc.s0:raw.zap.LG_spc.s1, :);
	raw.zapd.LG.egg = raw.cropd.LG.egg(raw.zap.LG_egg.s0:raw.zap.LG_egg.s1, :);
end

% Writing result
printf('[lmpkgRepack] Ready to export to ALN folders\n');
faln_us_wav  = sprintf('aln_%.4d/wd_%.4d_us.wav', data.misc.seq, data.misc.num);
faln_us_dv   = sprintf('aln_%.4d/wd_%.4d_us.dv',  data.misc.seq, data.misc.num);
faln_us_fea  = sprintf('aln_%.4d/wd_%.4d_us.ff', data.misc.seq, data.misc.num);

faln_cc_wav  = sprintf('aln_%.4d/wd_%.4d_cc.wav', data.misc.seq, data.misc.num);
faln_cc_fea  = sprintf('aln_%.4d/wd_%.4d_cc.ff', data.misc.seq, data.misc.num);
faln_cc_dv   = sprintf('aln_%.4d/wd_%.4d_cc.dv',  data.misc.seq, data.misc.num);

faln_ag_wav  = sprintf('aln_%.4d/wd_%.4d_ag.wav', data.misc.seq, data.misc.num);
faln_ag_pos  = sprintf('aln_%.4d/wd_%.4d_ag.pos', data.misc.seq, data.misc.num);
faln_ag_amp  = sprintf('aln_%.4d/wd_%.4d_ag.amp', data.misc.seq, data.misc.num);

if(data.misc.status.LG)
	faln_lg_wav  = sprintf('aln_%.4d/wd_%.4d_lg.wav', data.misc.seq, data.misc.num);
end
faln_txt  = sprintf('aln_%.4d/wd_%.4d.txt', data.misc.seq, data.misc.num);

printf('       Writing US data\n');
wavwrite(raw.zapd.US.spc, data.raw.US.spc_rate, 16, faln_us_wav);
lmpkgWriteFea(raw.zapd.US.fea, faln_us_fea);

printf('       Writing US-DV data (via dd/lm_dd)\n');
us_f0 = min(raw.zapd.US.fea(:,1));
us_f1 = max(raw.zapd.US.fea(:,1));
us_df = us_f1 - us_f0 + 1;
system(sprintf('lm_dd seq_%.4d/wd_%.4d_us.dv %s %d %d %s', data.misc.seq, data.misc.num, faln_us_dv, us_f0, us_df, cmd_stdo));

printf('       Writing CC data\n');
wavwrite(raw.zapd.CC.spc, data.raw.CC.spc_rate, 16, faln_cc_wav);
lmpkgWriteFea(raw.zapd.CC.fea, faln_cc_fea)

printf('       Writing CC-DV data (via dd/lm_dd)\n');
cc_f0 = min(raw.zapd.CC.fea(:,1));
cc_f1 = max(raw.zapd.CC.fea(:,1));
cc_df = cc_f1 - cc_f0 + 1;
system(sprintf('lm_dd seq_%.4d/wd_%.4d_cc.dv %s %d %d %s', data.misc.seq, data.misc.num, faln_cc_dv, cc_f0, cc_df, cmd_stdo));

printf('       Writing AG data\n');
wavwrite(raw.zapd.AG.spc, data.raw.AG.spc_rate, 16, faln_ag_wav);
lmpkgWriteAmpPos(raw.zapd.AG.amp, faln_ag_amp);
lmpkgWriteAmpPos(raw.zapd.AG.pos, faln_ag_pos);

if(data.misc.status.LG)
	printf('       Writing LG data\n');
	raw.zapd.LG.all = [raw.zapd.LG.spc raw.zapd.LG.egg];
	wavwrite(raw.zapd.LG.all, data.raw.LG.spc_rate, 16, faln_lg_wav);
end

printf('       Writing TXT data\n');
fid = fopen(faln_txt, 'w');
fprintf(fid, '%s', data.misc.txt);
fclose(fid);

retval = raw;


function rawoff = lmpkgResampleOffset(alnoff, alnsr, rawsr, type)
	off_float = alnoff * rawsr / alnsr;
	off_int   = round(off_float);
	printf('[lmpkgResampleOffset] Offset for %s:\n', type);
	printf('                      %5.d Hz --> %.2f\n', alnsr, alnoff);
	printf('                      %5.d Hz --> %.2f --> %.2f\n', rawsr, off_float, off_int);
	rawoff = off_int;


function delta = lmpkgComputeSlowOffset (dataptr, datasr, rawsr, idxcol)
	s0 = 1;
	s1 = datasr/rawsr; 
	frame_min = min(dataptr(s0:s1, idxcol));
	frame_max = max(dataptr(s0:s1, idxcol));
	printf('[lmpkgComputeSlowOffset] Data frame: %d-%d (samples)\n', s0, s1);
	printf('                         Min/Max:    %d/%d (frames)\n', frame_min, frame_max);
	delta = dataptr(s0:s1, idxcol);
	min_s0 = min(find(delta==frame_min))
	min_s1 = max(find(delta==frame_min)) 
	
	max_s0 = min(find(delta==frame_max))
	max_s1 = max(find(delta==frame_max))


function lmpkgWriteFea (dataptr, file)
	rows = max(size(dataptr));
	cols = min(size(dataptr));
	fid = fopen(file, 'w');
	for i = 1:rows
		for j = 1:cols
			if (j == 1)
				fprintf(fid, '%d', dataptr(i,j));
			else
				fprintf(fid, '%.6f', dataptr(i,j));
			end
			if (j ~= numel(dataptr))
				fprintf(fid, ' ');
			end
		end
		fprintf(fid, '\n');
	end
	fclose(fid);

function lmpkgWriteAmpPos (dataptr, file)
	rows = max(size(dataptr));
	cols = min(size(dataptr));
	fid = fopen(file, 'w');
	for i = 1:rows
		for j = 1:cols
			fprintf(fid, '%.6f', dataptr(i,j));
			if (j ~= numel(dataptr))
				fprintf(fid, ' ');
			end
		end
		fprintf(fid, '\n');
	end
	fclose(fid);
