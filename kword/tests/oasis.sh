#!/bin/bash

# This script helps finding out problems in the OASIS loading/saving code,
# by converting .kwd -> .odt -> .kwd and comparing the initial and final .kwd files.
# We use the kwd format as a "dump" of the KWord data, to check if everything is correct
# in memory, but the point is of course to ensure that the .odt has all the information.

# To use this script, you need to change this path to an existing kwd file
# Don't use a relative path, dcopstart won't handle it
input=/mnt/devel/kde/kofficetests/documents/native/kword/frames/transparent_pixmaps.kwd

test -f "$input" || { echo "No such file $input"; exit 1; }

# Load KWD
appid=`dcopstart kword $input`
while `dcop $appid Document-0 isLoading` == "true"; do
    sleep 1;
done

# Save to KWD again (in case of changes in syntax etc.)
origfile=/tmp/oasisregtest-initial.kwd
dcop $appid Document-0 saveAs $origfile || exit 1
dcop $appid Document-0 waitSaveComplete || exit 1
test -f $origfile || exit 1

# Save to OASIS ODT
tmpoasisfile=/tmp/oasisregtest.odt
dcop $appid Document-0 setOutputMimeType "application/vnd.oasis.opendocument.text" || exit 1
dcop $appid Document-0 saveAs $tmpoasisfile || exit 1
dcop $appid Document-0 waitSaveComplete || exit 1
test -f $tmpoasisfile || exit 1

dcopquit $appid

# Load resulting ODT, convert to KWD
tmpnativefile=/tmp/oasisregtest-final.kwd
appid=`dcopstart kword $tmpoasisfile`
# openURL doesn't clear enough stuff from the previous doc. I get crashes when saving paraglayouts...
# [pointing to deleted styles]
#dcop $appid Document-0 openURL $tmpoasisfile || exit 1
while `dcop $appid Document-0 isLoading` == "true"; do
    sleep 1;
done
dcop $appid Document-0 setOutputMimeType "application/x-kword" || exit 1
dcop $appid Document-0 saveAs $tmpnativefile || exit 1
test -f $tmpnativefile || exit 1

# Compare initial and final KWD files
rm -rf /tmp/oasisregtest-orig
mkdir /tmp/oasisregtest-orig
rm -rf /tmp/oasisregtest-final
mkdir /tmp/oasisregtest-final
rm -rf /tmp/oasisregtest-oasis
mkdir /tmp/oasisregtest-oasis
cd /tmp/oasisregtest-orig
unzip $origfile || exit 1
cd /tmp/oasisregtest-final
unzip $tmpnativefile || exit 1
cd /tmp/oasisregtest-oasis
unzip $tmpoasisfile || exit 1
cd ..

# Some fixups
# 1) modification time obviously changed, remove it
# 2) the name of the main text frameset changes, no big deal, so adjust in orig
perl -pi -e 's/modificationDate=\"[0-9-T:]*\"//;s/\"Text Frameset 1\"/\"Main Text Frameset\"/' oasisregtest-orig/maindoc.xml
perl -pi -e 's/modificationDate=\"[0-9-T:]*\"//' oasisregtest-final/maindoc.xml

diff -urp oasisregtest-orig oasisregtest-final 2>&1 | tee oasisdiff | less

echo "See /tmp/oasisregtest-oasis for the OASIS xml files."

