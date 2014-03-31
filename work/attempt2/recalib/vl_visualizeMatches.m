function [M] = vl_visualizeMatches(imgA, kptsA, imgB, kptsB, matches, scores, thresh)
    % imgA = B/W image 1
    % kptsA : kpts detected from image A using vl_sift => F
    % img B, kptsB <similar to A>
    % matches, scores : from vl_ubcmatch
    % thresh : show matches only above a given threshold (set as 100)
    X = figure(1); clf;
    M = appendimages(imgA, imgB);
    imagesc(M);
    hold on;
    for i = 1 : size(matches, 2)
        if (scores(i) < thresh)
            continue;
        end
        pos1 = [kptsA(1, matches(1, i)); kptsA(2, matches(1, i))];
        %plot(pos1(1), pos1(2), 'g.');
        pos2 = [size(imgA,2) + kptsB(1, matches(2, i)); kptsB(2, matches(2, i))];
        %plot(pos2(1), pos2(2), 'g.');
        line([pos1(1) pos2(1)], [pos1(2) pos2(2)]);
    end
    axis image off;
    drawnow;
    saveTightFigure(X, 'matches.jpg');
