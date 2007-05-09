function data = lmCollectData(AG, file_amp, file_pos)
% Usage:
%   data = lmCollectData(AG, file_amp, file_pos)

data = [];
data_amp = lmLoadData(AG, file_amp);
data_pos = lmLoadData(AG, file_pos);

if (data_amp.samples == data_pos.samples)
	data = data_pos;
	data.amp = data_amp.amp;
	clear data_amp;
	clear data_pos;
end
