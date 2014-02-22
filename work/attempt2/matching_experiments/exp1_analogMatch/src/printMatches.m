function printMatches(fname, matchPoints1, matchPoints2)
% Format of matchPoints : n rows, each row with x, y coords
f = fopen(fname, 'w');
for i = 1:size(matchPoints1,1)
    fprintf(f, '%d %d %d %d\n', matchPoints1(i, 1), matchPoints1(i, 2), ...
        matchPoints2(i, 1), matchPoints2(i, 2));
end
fclose(f);