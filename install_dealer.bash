#!/bin/bash
# File install.bash -- JGM -- 2022-03-11

DISTRODIR="$PWD"
ROOTDIR="/usr/local/bin"

if [[ -d "$ROOTDIR"/DealerV2 ]]; then echo Found "$ROOTDIR"/DealerV2
else
   echo Making "$ROOTDIR"/DealerV2
   mkdir "$ROOTDIR"/DealerV2
fi
if [[ -d "$ROOTDIR"/DOP ]] ; then echo Found "$ROOTDIR"/DOP
else
   echo Making "$ROOTDIR"/DOP
   mkdir "$ROOTDIR"/DOP
fi

# Copy the Relevant Repo Contents to the Install Dir; preserve permission bits
cp -pR "$DISTRODIR"/*  "$ROOTDIR"/DealerV2
chown -R $LOGNAME:$LOGNAME "$ROOTDIR"/DealerV2/*

# setup the DOP Perl external program
mv "$ROOTDIR"/DealerV2/DOP/*  "$ROOTDIR"/DOP/
rmdir  "$ROOTDIR"/DealerV2/DOP

echo Adding /usr/local/bin/Dealer /usr/local/bin/DOP to PATH
echo "PATH=/usr/local/bin/Dealer:usr/local/bin/DOP:$PATH" >> ~/.bashrc

# setup the GIB external program in case there is anyone who still wants to use it.
cp -p /usr/games/bridge /usr/games/gibcli

echo Dealver2 installed in "$ROOTDIR"/DealerV2 and the OPC Perl script in "$ROOTDIR"/DOP


