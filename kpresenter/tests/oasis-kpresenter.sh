#!/bin/bash

# This script helps finding out problems in the OASIS loading/saving code,
# by converting .kpr -> .odp -> .kpr and comparing the initial and final .kpr files.
# We use the kpr format as a "dump" of the KPresenter data, to check if everything is correct
# in memory, but the point is of course to ensure that the .odt has all the information.

# To use this script, you need to change this path to an existing kwd file
# Don't use a relative path, dcopstart won't handle it
#input=/mnt/devel/kde/kofficetests/documents/native/kpresenter/test.kpr
input=/home/laurent/velo.kpr

test -f "$input" || { echo "No such file $input"; exit 1; }

# Load KPR
appid=`dcopstart kpresenter $input`
while `dcop $appid Document-0 isLoading` == "true"; do
    sleep 1;
done

# Save to KPR again (in case of changes in syntax etc.)
origfile=/tmp/oasisregtest-initial.kpr
dcop $appid Document-0 saveAs $origfile || exit 1
dcop $appid Document-0 waitSaveComplete || exit 1
test -f $origfile || exit 1

# Save to OASIS ODT
tmpoasisfile=/tmp/oasisregtest.odp
dcop $appid Document-0 setOutputMimeType "application/vnd.oasis.opendocument.presentation" || exit 1
dcop $appid Document-0 saveAs $tmpoasisfile || exit 1
dcop $appid Document-0 waitSaveComplete || exit 1
test -f $tmpoasisfile || exit 1

dcopquit $appid

# Load resulting ODP, convert to KPR
tmpnativefile=/tmp/oasisregtest-final.kpr
appid=`dcopstart kpresenter $tmpoasisfile`
# openURL doesn't clear enough stuff from the previous doc. I get crashes when saving paraglayouts...
# [pointing to deleted styles]
#dcop $appid Document-0 openURL $tmpoasisfile || exit 1
while `dcop $appid Document-0 isLoading` == "true"; do
    sleep 1;
done
dcop $appid Document-0 setOutputMimeType "application/x-kpresenter" || exit 1
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


diff -urp oasisregtest-orig oasisregtest-final 2>&1 | tee oasisdiff | less

echo "See /tmp/oasisregtest-oasis for the OASIS xml files."

