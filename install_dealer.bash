# File install.bash -- JGM -- 2022-03-11

DISTRODIR="$PWD"
ROOTDIR="/usr/local/bin"
DISTRODIR="/home/greg19/Programming/Bridge_SW/JGMDealer/Dealer-Version-2-"
ROOTDIR="/tmp/local/bin"
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
# setup the DOP Perl external program
cp -p "$DISTRODIR"/DOP4DealerV2.tar.gz "$ROOTDIR"/DOP
(cd "$ROOTDIR"/DOP && tar  -zxf DOP4DealerV2.tar.gz)
ln -s "$ROOTDIR"/DOP/dop  "$ROOTDIR"/DOP/DOP
chown $LOGNAME:$LOGNAME "$ROOTDIR"/DOP/*
cd "$ROOTDIR"/DOP
   chmod 555 *.pl dop DOP
   chmod 444 Copyright_Notice.txt GPL* OPC*
   chmod 444 Examples/* Doc/*
cd "$ROOTDIR"
# Copy the Relevant Repo Contents to the Install Dir; Set ownership and write protect some files
cp -pR "$DISTRODIR"/*  "$ROOTDIR"/DealerV2
ln -s  "$ROOTDIR"/DealerV2/Prod/dealerv2 "$ROOTDIR"/dealerv2
chown -R $LOGNAME:$LOGNAME "$ROOTDIR"/DealerV2/*
cd "$ROOTDIR"/DealerV2
chmod 444 README.md LICENSE Prod/* Debug/* src/* include/* docs/* Examples/Descr*
chmod 555 dealerv2 Prod/dealer Debug/dealdbg
(cd lib && chmod 444 lib* README* )
(cd lib && chmod 555 Dist.pm fdp fdpi )
for dir in docs src include Examples stdlib ; do
  chmod 444 "$dir"/*
done
chmod 555 Prod/dealerv2 Debug/dealdbg

echo Adding /usr/local/bin/Dealer /usr/local/bin/DOP to PATH
echo "PATH=/usr/local/bin/Dealer:usr/local/bin/DOP:$PATH" >> ~/.bashrc

echo Dealver2 installed in "$ROOTDIR"/DealerV2 and the OPC Perl script in "$ROOTDIR"/DOP


