#!/bin/bash
if [ $# -lt 1 ]
then
    echo 'Usage:' $0 'function_name arguments'
    exit
fi

#TMPDIR='tmp/'
matlab_exec=matlab
X="${1}(${2})"
TMP_FNAME=matlab_cmd_${1}.m
mkdir -p $TMPDIR
echo ${X} > $TMPDIR/$TMP_FNAME
cat $TMPDIR/$TMP_FNAME
${matlab_exec} -nojvm -nodisplay -nosplash < $TMPDIR/$TMP_FNAME
rm $TMPDIR/$TMP_FNAME
