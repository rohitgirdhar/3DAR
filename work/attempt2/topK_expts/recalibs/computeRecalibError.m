function err = computeRecalibError(im1_id, im2_id)
% Returns the error when im1 is recalibrated using im2
I = imread(strcat('../../Images_sc/', im1_id, '.jpg'));
P = imread(strcat('../../Images_sc/', im2_id, '.jpg'));
[f1, d1] = vl_sift(single(rgb2gray(I)));
[f2, d2] = vl_sift(single(rgb2gray(P)));
disp('found sift');
matches = vl_ubcmatch(d1, d2);
disp('computed matching');
if size(matches, 2) > 6
    [~, inliers] = computeF(f1, f2, matches);
    matches = filterMatches(matches, inliers);
else
    disp(sprintf('Insufficient matches for geom verify (%d)', size(matches,2)));
end
disp('geometric filtered');
%vl_visualizeMatches(I, f1, P, f2, matches);
[mp1, mp2] = getMatchingPoints(f1, f2, matches);
data = dlmread(strcat('ptcorrs/', im2_id, '.ptcorr'));
Wpt = data(:, 1:3);
Ipt = data(:, 4:5);
Ipt = [Ipt(:, 1) + 1024.0/2, Ipt(:, 2) + 768.0/2];
[X, x] = find2d2dMatching(Ipt, Wpt, mp2, mp1);
X = [X, ones(size(X,1), 1)]';
x = [x, ones(size(x,1), 1)]';
[K T w] = estimate_camera_DLT(x, X);
%K = calibration_matrix(K);
%R = rodrigues(omega);
%ProjMat = K * [R, T]
err = error_reproj(x, K, T, w, X);
disp('done');
