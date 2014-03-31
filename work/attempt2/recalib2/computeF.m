function [F, inliers] = computeF(kp1, kp2, matches)
[matchedPoints1, matchedPoints2] = getMatchingPoints(kp1, kp2, matches);
[F, inliers] = estimateFundamentalMatrix(matchedPoints1, matchedPoints2);