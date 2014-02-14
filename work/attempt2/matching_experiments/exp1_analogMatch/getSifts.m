function allSifts = getSifts(path, N)
    % path: the path to folder with files (visualize dir)
    % N = total number of images at path
    %matlabpool open 4;
    allSifts = [];
    
    scaleDownRat = 1;
    
    parfor i = 0:N-1
        name = sprintf('%08d', i);
        I = imread(strcat(path, name, '.jpg'));
        I = imresize(I, scaleDownRat);
        [~, D] = vl_sift(single(rgb2gray(I)));
        allSifts = [allSifts, D];
        strcat('Done for ', name)
    end