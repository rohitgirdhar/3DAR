#!/bin/bash
for ((i=15; i<=81; i=i+5))
do
    err=0
    sim=0
    num_retries=1 # useful for calculating random, else set = 1
    for ((j=0; j<$num_retries; j++))
    do
        res=`python selectGreedy.py -f E_bob.txt -o res.txt -t test_set -K $i -n E_bob_norm.txt`
        err_j=`echo $res | cut -d' ' -f1`
        err=`bc <<< "scale=6;$err + $err_j"`
        sim_j=`echo $res | cut -d' ' -f2`
        sim=`bc <<< "scale=6;$sim + $sim_j"`
    done
    err=`bc <<< "scale=6;$err / $num_retries"`
    sim=`bc <<< "scale=6;$sim / $num_retries"`
    errs="$errs,[$i,$err]"
    sims="$sims,[$i,$sim]"
done
echo $errs
echo $sims
