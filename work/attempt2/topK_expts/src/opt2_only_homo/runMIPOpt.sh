#!/bin/bash

# prints in 0 indexed, in accordance greedy method

if [ $# -lt 1 ]; then
    echo 'Use ./prog <LP file path>'
    exit -1
fi
TMPFILE=`mktemp`
printf "read ${1}\noptimize\ndisplay solution\n" > $TMPFILE
lst=`~/optimization/scip-3.1.0.linux.x86_64.gnu.opt.spx < $TMPFILE | grep x_ | cut -d '_' -f2 | cut -d' ' -f1`
for i in $lst; do
    echo `expr $i - 1`
done
rm $TMPFILE
