#!/bin/bash
REFDIR=${1:-.}
err=0
for of in Output/*.out ; do
   FN=$( basename $of )
   FN=${FN%%.out} ;
   rf="$REFDIR/${FN}.ref";
   fileno=$(( $fileno + 1 ))
   echo "$fileno.    copy $of to $rf"
   cp $of $rf
 done
   echo Copied $fileno .out files to $REFDIR .ref files


