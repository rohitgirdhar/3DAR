function matches_filtered = filterMatches(matches, inliers)
matches_filtered = zeros(2, sum(inliers));
cntr = 1;
for i = 1 : size(matches, 2)
    if inliers(i) == 1
        matches_filtered(:, cntr) = matches(:, i);
        cntr = cntr + 1;
    end
end