function vl_siftNmatch(I, P, good)
    Ibw = single(rgb2gray(I));
    Pbw = single(rgb2gray(P));
    [fI, dI] = vl_sift(Ibw);
    [fP, dP] = vl_sift(Pbw);
    [matches, scores] = vl_ubcmatch(dI, dP);
    
    % Optional, geometric verification
    if good
        Ip = fI(1:2, matches(1, :));
        Pp = fP(1:2, matches(2, :));
        [~, inliers] = estimateFundamentalMatrix(Ip', Pp');
        matches2 = [];
        for i = 1 : size(matches, 2)
            if inliers(i) == 1
                matches2 = [matches2, matches(:, i)];
            end
        end
        matches = matches2;
    end
    
    vl_visualizeMatches(I, fI, P, fP, matches, scores, 100);