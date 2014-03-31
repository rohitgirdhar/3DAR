function [S,dSdT] = skew3(T)

S = [   0  -T(3)  T(2)
      T(3)   0   -T(1)
     -T(2)  T(1)   0 ];

dSdT = [0 0 0;0 0 1;0 -1 0 ;0 0 -1;0 0 0;1 0 0 ;0 1 0;-1 0 0; 0 0 0];

return;