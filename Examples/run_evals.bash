#!/bin/bash
for contract in z4H z4S z5D  ; do
   # echo Running Dealer to eval $contract
   ../Prod/dealerv2 -m   -v -1 "$contract"  Descr.scrEvals    
 done

