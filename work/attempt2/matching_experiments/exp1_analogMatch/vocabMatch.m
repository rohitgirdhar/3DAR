function matches = vocabMatch(descrs1, descrs2, cluster_centers)
    % descrs1, 2 are matrices, where each column is a 128D sift vector
    % cluster_center is obtained from vl_ikmeans (C)
    % returns matches, where each column is 2-row vector,
    %   with (idx from first image, idx from second image)
    matches = [];
    vw2 = [];
    for i = 1 : size(descrs2, 2)
        vw2 = [vw2, vl_ikmeanspush(descrs2(:, i), cluster_centers)];
    end
    for i = 1 : size(descrs1, 2)
        descr = descrs1(:, i);
        desc_vw = vl_ikmeanspush(descr, cluster_centers);
        for j = 1 : size(vw2, 2)
            if vw2(1, j) == desc_vw
                matches = [matches, [i ; j] ];
            end
        end
    end