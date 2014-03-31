function vl_runAll(img1_path, img2_path)
% Computes the matching and stores in matches.txt file

run('~/startup.m');
I = imread(img1_path);
P = imread(img2_path);

vl_siftNmatch(I, P, 1);
