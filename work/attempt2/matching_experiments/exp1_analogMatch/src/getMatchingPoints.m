function [matchedPoints1, matchedPoints2] = getMatchingPoints(kp1, kp2, matches)
num_matches = size(matches, 2);
matchedPoints1 = zeros(num_matches, 2);
matchedPoints2 = zeros(num_matches, 2);
for i = 1:size(matches, 2)
    matchedPoints1(i, :) = [kp1(1, matches(1,i)), kp1(2, matches(1,i))];
    matchedPoints2(i, :) = [kp2(1, matches(2,i)), kp2(2, matches(2,i))];
end    