function [v_out m M] = mtNormalize (v_in, ll, ul);
% function [out m M] = m_norm (I, ll, ul)

m = min (min (v_in));
M = max (max (v_in));

v_out = (v_in - m) * (ul - ll) ./ (M - m);
