function matches = vocabMatch(descrs1, descrs2, cluster_centers)
    % descrs1, 2 are matrices, where each column is a 128D sift vector
    % cluster_center is obtained from vl_hikmeans (C)
    % returns matches, where each column is 2-row vector,
    %   with (idx from first image, idx from second image)
    matches = [];
    vw2 = [];
    for i = 1 : size(descrs2, 2)
        vw2 = [vw2, getWord(vl_hikmeanspush(cluster_centers, uint8(descrs2(:, i))), 10)];
    end
    for i = 1 : size(descrs1, 2)
        descr = descrs1(:, i);
        desc_vw = getWord(vl_hikmeanspush(cluster_centers, uint8(descr)), 10);
        for j = 1 : size(vw2, 2)
            if vw2(1, j) == desc_vw
                matches = [matches, [i ; j] ];
            end
        end
    end
    disp(sprintf('Found %d matches', size(matches, 2)));

function w1 = getWord(path, branch_factor)
    w1 = 0;
    for i = 1 : size(path)
        w1 = (w1 * branch_factor) + path(i) - 1;
    end 
