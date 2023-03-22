#!/bin/bash
# Use this in the sam
for contract in z3N z4H z4Sx z5Dxx  ; do
   # echo Running Dealer to eval $contract
   dealerv2 -m  -p100 -v -1 "$contract"  ../Eval_scriptedContract.dls
 done

