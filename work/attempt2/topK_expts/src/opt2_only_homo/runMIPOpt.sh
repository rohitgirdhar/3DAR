#!/bin/bash

# converts to 0 indexed, in accordance greedy method

if [ $# -lt 2 ]; then
    echo 'Use ./prog <LP file path> <Img names (ordered)>'
    exit -1
fi
TMPFILE=`mktemp`
TMPFILE2=`mktemp`
printf "read ${1}\noptimize\ndisplay solution\n" > $TMPFILE
lst=`~/optimization/scip-3.1.0.linux.x86_64.gnu.opt.spx < $TMPFILE | grep x_ | cut -d '_' -f2 | cut -d' ' -f1`
for i in $lst; do
    num_re='^[0-9]+$'
    if [[ $i =~ $num_re ]]; then
        echo `expr $i - 1` | cat >> $TMPFILE2
    fi
done
python translateImgIds.py $TMPFILE2 $2
rm $TMPFILE
rm $TMPFILE2
