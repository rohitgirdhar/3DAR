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
    
       
    % FORMAT : <I 2d pt> <P 2d pt> score
    f = fopen('matches.txt', 'w');
    for i = 1 : size(matches, 2)
        src = matches(1, i);
        dst = matches(2, i);
        fprintf(f, '%f %f %f %f %f\n', fI(1, src), fI(2, src), fP(1, dst), fP(2, dst), scores(i));
    end
    fclose(f);
    
    vl_visualizeMatches(I, fI, P, fP, matches, scores, 100);
