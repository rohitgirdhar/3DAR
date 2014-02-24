function runMatching(im1_id, im2_id)
run('~/vision/vlfeat/toolbox/vl_setup.m');
%load '../data/voc_10000.mat'
load '../data/tree_voc_10.mat'
I = imread(strcat('../../../Images_sc/', im1_id, '.jpg'));
P = imread(strcat('../../../Images_sc/', im2_id, '.jpg'));
[f1, d1] = vl_sift(single(rgb2gray(I)));
[f2, d2] = vl_sift(single(rgb2gray(P)));
disp('found sift');
%matches = vocabMatch(d1, d2, clusterCenters);
matches = vocabMatch_hik(d1, d2, clusterCenters);
disp('computed matching');
if size(matches, 2) > 6
    [~, inliers] = computeF(f1, f2, matches);
    filter_matches = filterMatches(matches, inliers);
else
    disp(sprintf('Insufficient matches for geom verify (%d)', size(matches,2)));
    filter_matches = matches;
end
disp('geometric filtered');
%vl_visualizeMatches(I, f1, P, f2, filter_matches);
[mp1, mp2] = getMatchingPoints(f1, f2, filter_matches);
printMatches('m.txt', mp1, mp2);
disp('done');
