#!/bin/bash
PATH=/bin:$PATH
SUFFIX_OLD=fisher
SUFFIX_NEW=xray
SUFFIX_OLD_CAP=FISHER
SUFFIX_NEW_CAP=XRAY

MOD_OLD=mod_$SUFFIX_OLD
MOD_NEW=mod_$SUFFIX_NEW
MOD_OLD_CAP=MOD_$SUFFIX_OLD_CAP
MOD_NEW_CAP=MOD_$SUFFIX_NEW_CAP
MOD_OLD_CLASS=Mod_$SUFFIX_OLD
MOD_NEW_CLASS=Mod_$SUFFIX_NEW

rm -f *.aps
find . -name CVS -exec rm -rf {} \;
for i in `find . -name $MOD_OLD\*`; do
  NEWNAME=`echo $i|sed s/$MOD_OLD/$MOD_NEW/g`
  mv $i $NEWNAME
done;
for i in `find . -type f|grep -v convert.sh`; do
  sed s/$MOD_OLD/$MOD_NEW/g < $i > a.tmp
  mv a.tmp $i

  sed s/$MOD_OLD_CAP/$MOD_NEW_CAP/g < $i > a.tmp
  mv a.tmp $i

  sed s/$MOD_OLD_CLASS/$MOD_NEW_CLASS/g < $i > a.tmp
  mv a.tmp $i
  
  unix2dos $i
done