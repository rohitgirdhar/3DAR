function runRecalibCompute(img_idx)
run('~/vision/vlfeat/toolbox/vl_setup');
run('~/vision/vlg/toolbox/vlg_setup');
OUTPUT_DIR = 'results/';
OUTF = strcat(OUTPUT_DIR, 'recalib_', int2str(img_idx), '.txt');
delete(OUTF);
N = 285;
Iidx = sprintf('%08d', img_idx);
for j = 0 : N-1
        Pidx = sprintf('%08d', j);
        try
            err = computeRecalibError(Iidx, Pidx);
        catch
            disp('Error in computing recalib error. Setting to NaN');
            err = NaN;
        end
        f = fopen(OUTF, 'a');
        fprintf(f, '%f\n', err);
        fclose(f);
        disp(sprintf('Done for %d -> %d : %f', img_idx, j, err));
end
