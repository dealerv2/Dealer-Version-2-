#!/bin/bash
REFDIR=${1:-.}
err=0
for of in Output/*.out ; do
   FN=${of%%.out} ;
   rf="$REFDIR/${FN}.ref";
   fileno=$(( $fileno + 1 ))
   echo "$fileno.    comparing $of to $rf"
   diff $of $rf
   if [[ $? != 0 ]] ; then err=$(( $err + 1 )) ; echo $err ; fi
done
echo There were $err files with differences out of $fileno compares


