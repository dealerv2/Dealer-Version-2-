#!/usr/bin/bash
for hdr in $( ls ../include/*.h ) ; do
#echo  $hdr found in:
grep $hdr *.c
echo
done
