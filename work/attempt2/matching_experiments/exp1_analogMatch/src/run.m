sifts = getSifts('../../Images_sc/', 285);
save('allsifts.mat', sifts);
[C,A] = vl_kmeans(double(sifts), 10);
