function res = computeOverlap(I, J)
% Computes the % overlap in images I and J (black vs non), w.r.t I

I = im2bw(rgb2gray(I), 0.1);
J = im2bw(rgb2gray(J), 0.1);

overlap = I & J;
res = sum(sum(overlap)) * 100 / sum(sum(I));