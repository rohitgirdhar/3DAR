#!/bin/bash

TMPDIR='tmp/'
matlab_exec=matlab
X="${1}(${2})"
mkdir -p $TMPDIR
echo ${X} > $TMPDIR/matlab_command_${2}.m
cat $TMPDIR/matlab_command_${2}.m
${matlab_exec} -nojvm -nodisplay -nosplash < $TMPDIR/matlab_command_${2}.m
rm $TMPDIR/matlab_command_${2}.m
