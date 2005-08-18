#!/bin/bash

# This script helps finding out problems in the OASIS loading/saving code,
# by converting .kpr -> .odp -> .kpr and comparing the initial and final .kpr files.
# We use the kpr format as a "dump" of the KPresenter data, to check if everything is correct
# in memory, but the point is of course to ensure that the .odp has all the information.

# To use this script, you need to pass the full path to an existing kpr file as argument.
# Don't use a relative path, dcopstart won't handle it
input="$1"

# Set this to 1 in order to validate the saved oasis document using oasislint
checkoasis="1"

appname=kpresenter
oldextension=kpr
oasisextension=odp
oasismimetype=application/vnd.oasis.opendocument.presentation

test -f "$input" || { echo "No such file $input"; exit 1; }

# Load old native file
appid=`dcopstart $appname $input`
test -n "$appid" || { echo "Error starting $appname!"; exit 1; }
while `dcop $appid Document-0 isLoading` == "true"; do
    sleep 1;
done

# Save again (in case of changes in syntax etc.)
origfile=$PWD/oasisregtest-initial.$oldextension
dcop $appid Document-0 saveAs $origfile || exit 1
test -f $origfile || exit 1

# Save to OASIS
tmpoasisfile=$PWD/oasisregtest.$oasisextension
dcop $appid Document-0 setOutputMimeType $oasismimetype || exit 1
dcop $appid Document-0 saveAs $tmpoasisfile || exit 1
test -f $tmpoasisfile || exit 1

dcopquit $appid

# Load resulting OASIS file, convert to old native format
tmpnativefile=$PWD/oasisregtest-final.$oldextension
appid=`dcopstart $appname $tmpoasisfile`
while `dcop $appid Document-0 isLoading` == "true"; do
    sleep 1;
done
dcop $appid Document-0 setOutputMimeType "application/x-$appname" || exit 1
dcop $appid Document-0 saveAs $tmpnativefile || exit 1
test -f $tmpnativefile || exit 1

# Unpack everything
rm -rf oasisregtest-orig
mkdir oasisregtest-orig
rm -rf oasisregtest-final
mkdir oasisregtest-final
rm -rf oasisregtest-oasis
mkdir oasisregtest-oasis
cd oasisregtest-orig || exit 1
unzip $origfile || exit 1
cd ..
cd oasisregtest-final || exit 1
unzip $tmpnativefile || exit 1
cd ..
# Validate OASIS format
cd oasisregtest-oasis || exit 1
unzip $tmpoasisfile || exit 1
if test "$checkoasis" = "1"; then
  if type -p oasislint >/dev/null 2>&1; then
    for f in content.xml styles.xml meta.xml settings.xml; do
      echo "Checking $f..." ; oasislint $f 
    done
  fi
  if type -p oasislint-strict >/dev/null 2>&1; then
    for f in content.xml styles.xml meta.xml settings.xml; do
      echo "Checking $f strict..." && oasislint-strict $f
    done
  fi
fi
cd ..

# Compare initial and final "native format" files
diff -urp oasisregtest-orig oasisregtest-final 2>&1 | tee oasisdiff | less

echo "See oasisregtest-oasis for the OASIS xml files."
echo "For a better diffing mechanism, launch xemacs and paste into a terminal:"
echo "gnudoit '(ediff-files \"$PWD/oasisregtest-orig/maindoc.xml\" \"$PWD/oasisregtest-final/maindoc.xml\")'"
