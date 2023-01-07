#!/bin/bash
# File install.bash -- JGM -- 2022-03-11
#set -x
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
chown -R ${SUDO_USER}:${SUDO_USER} "$ROOTDIR"/DealerV2/*

# setup the DOP Perl external program
mv "$ROOTDIR"/DealerV2/DOP/*  "$ROOTDIR"/DOP/
rmdir  "$ROOTDIR"/DealerV2/DOP

if $( echo $PATH | grep /usr/local/bin/DealerV2 | grep -q /usr/local/bin/DOP ) ; then
   echo PATH is OK
else
   echo Adding /usr/local/bin/DealerV2 /usr/local/bin/DOP to PATH
   echo 'PATH=/usr/local/bin/DealerV2:usr/local/bin/DOP:$PATH' >> ~/.bashrc
fi

# setup the GIB external program in case there is anyone who still wants to use it.
cp -p /usr/games/bridge /usr/games/gibcli

# make sure the exec permission bits are set for the script files
chmod +x /usr/games/bridge /usr/games/gibcli
chmod +x ${ROOTDIR}/DOP/*.pl ${ROOTDIR}DealerV2/lib/fdp*

echo installed the OPC Perl script(s) in "$ROOTDIR"/DOP
echo dealver2 installed in "$ROOTDIR"/DealerV2,  and the fdpi and fdp Perl script(s) in "$ROOTDIR"/DealerV2/lib


