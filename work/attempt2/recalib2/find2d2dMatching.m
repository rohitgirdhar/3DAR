function [W, I] = find2d2dMatching(known2dpts, known3dpts, ext2dpts, matching2dpts)
% args:
% known2dpts: nx2 vector of pts whose 3D pt is known, on img 2
% known3dpts: nx3 vector of corr 3D pts to known2dpts
% ext2dpts: nx2 vector of pts that were extracted on img2
% matching2dpts: nx2 vector of pts, MATCHING to ext2dpts, on img1
% return:
% W: mx3 array of 3D points
% I: mx2 array of corr 2D points

bin_size = 1.0;
nbins_X = 1024.0 / bin_size + 1;
nbins_Y = 768.0 / bin_size + 1;
Wpts = zeros(nbins_X, nbins_Y, 3);

for pt = 1 : size(known2dpts)
    X = uint32(known2dpts(pt, 1) / bin_size);
    Y = uint32(known2dpts(pt, 2) / bin_size);
    Wpts(X, Y, :) = known3dpts(pt, :);
end

W = [];
I = [];
for pt = 1 : size(ext2dpts)
    X = uint32(ext2dpts(pt, 1) / bin_size);
    Y = uint32(ext2dpts(pt, 2) / bin_size);
    temp = reshape(Wpts(X,Y,:), 3, 1);
    if (norm(temp) > 0)
        W = [W; temp'];
        I = [I; matching2dpts(pt, :)];
    end 
end
