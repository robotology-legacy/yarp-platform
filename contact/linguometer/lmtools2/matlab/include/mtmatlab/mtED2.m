function d = mtED2 (A, B)
%function d = mtED2 (A, B)
%returns the squared euclidean distance

if (length(A) ~= length(B))
	error('Points must have the same length\n');
end

d = 0;
for i = 1:length (A)
	d = d + (A(i) - B(i))^2;
end
