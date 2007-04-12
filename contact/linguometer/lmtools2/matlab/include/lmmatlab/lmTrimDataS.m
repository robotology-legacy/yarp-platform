function data_out = lmTrimDataS (AG, data_in, s0, s1);
% Info:  trims data_in and fixes data_out
% Usage: data_out = mFixData (AG, data_in)
% Where: AG        profile
%        data_in    input data
%        s0        from sample s0...
%        s1        ... to sample s1

if (s0 < 1 || s1 > data_in.samples)
	error ('[lmTrimDataS] Trim error');
end

printf ('[lmTrimDataS] Trimming data: %d to %d samples\n', s0, s1);
data_out= [];

pos = data_in.pos;
amp = data_in.amp;

if (isempty(pos) ==0 && isempty(amp == 0))
	if (length(pos) ~= length(amp))
		error ('[lmTrimDataS] POS and AMP data: size error!');
	end
end
if (isempty(pos) && isempty(amp))
	error ('[lmTrimDataS] POS and AMP data: consistency error!');
end

if (isempty(amp) == 0)
	amp = amp (s0:s1, :, :);
end

if (isempty(pos) == 0)
	pos = pos (s0:s1, :, :);
end

if (isempty(amp) == 0)
	data_out = lmFixData (AG, amp);
	if (isempty(pos) == 0)
		data_out.pos = pos;
	else
		data_out.pos = [];
	end
elseif (isempty(pos) == 0)
	data_out = lmFixData (AG, pos);
	if (isempty(amp) == 0)
		data_out.amp = amp;
	else
		data_out.amp = [];
	end
end



%data_out.time.t0 = data_out.time.t0 + mSamplesToClock (AG, s0 - 1);
%data_out.time.t1 = data_out.time.t0 + mSamplesToClock (AG, s1 - s0);
%data_out.time.domain = [data_out.time.t0:1/AG.pars.f/AG.pars.tf:data_out.time.t1];

