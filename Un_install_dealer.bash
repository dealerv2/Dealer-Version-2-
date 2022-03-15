#!/bin/bash
# File UN-install.bash -- JGM -- 2022-03-11
#set -x
echo "This will Uninstall DealerV2 and delete all the files in /usr/local/bin/DealerV2 and /usr/local/bin/DOP!!"
read -rp "Continue? [Yn]" ANSW
if [[ $ANSW == "Y" || $ANSW == "Yes" ]] ; then
   echo "Bombs Away !"
else echo "Safe choice!" ; exit ; fi

#DISTRODIR="$PWD"
ROOTDIR="/usr/local/bin"
cd $ROOTDIR
chmod -R 777 DealerV2/* DOP/*
echo Removing "$ROOTDIR"/DealerV2 and all its files
rm -R DealerV2/*
rmdir DealerV2
echo Removing "$ROOTDIR"/DOP and all its files
rm -R DOP/*
rmdir DOP

echo Dealver2 and the OPC Perl script Un-installed
