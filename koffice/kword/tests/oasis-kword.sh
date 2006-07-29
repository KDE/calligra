#!/bin/bash

# This script helps finding out problems in the OASIS loading/saving code,
# by converting .kwd -> .odt -> .kwd and comparing the initial and final .kwd files.
# We use the kwd format as a "dump" of the KWord data, to check if everything is correct
# in memory, but the point is of course to ensure that the .odt has all the information.

# To use this script, you need to pass the path to an existing kwd file as argument.
input="$1"

# Set this to 1 in order to validate the saved oasis document using oasislint
checkoasis="1"

appname=kword
oldextension=kwd
oasisextension=odt
oasismimetype=application/vnd.oasis.opendocument.text

# dcopstart won't handle a relative path (kdeinit has a different cwd)
# so make it absolute
if echo "$input" | grep -v '^/' >/dev/null 2>&1; then
  input="$PWD/$input"
fi

test -f "$input" || { echo "No such file $input"; exit 1; }

# Load old native file
appid=`dcopstart $appname $input`
test -n "$appid" || { echo "Error starting $appname!"; exit 1; }
while test "`dcop $appid Document-0 isLoading`" != "false"; do
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

rm -rf oasisregtest-oasis
mkdir oasisregtest-oasis
cd oasisregtest-oasis || exit 1
unzip $tmpoasisfile || exit 1
cd ..

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
cd oasisregtest-orig || exit 1
unzip $origfile || exit 1
cd ..
cd oasisregtest-final || exit 1
unzip $tmpnativefile || exit 1
cd ..
# Validate OASIS format
cd oasisregtest-oasis || exit 1
if test "$checkoasis" = "1"; then
  if type -p oasislint >/dev/null 2>&1; then
    for f in content.xml styles.xml meta.xml settings.xml; do
      echo "Checking $f..." ; oasislint $f 
    done
  fi
  # Hide warning about non-standard oasis attribute being used for a kword-specific feature
  perl -pi -e 's/style:frame-behavior-on-new-page="[^\"]*"//' content.xml
  if type -p oasislint-strict >/dev/null 2>&1; then
    for f in content.xml styles.xml meta.xml settings.xml; do
      echo "Checking $f strict..." && oasislint-strict $f
    done
  fi
fi
cd ..

# Some fixups - kword specific
# 1) the name of the main text frameset changes, no big deal, so adjust in orig
# 2) modification time obviously changed, remove it
# 3) paragraph direction, text color and relativetextsize are now explicitly saved, hide them from the diff
perl -pi -e 's/\"Text Frameset 1\"/\"Main Text Frameset\"/;s,<WEIGHT value="48" />,<WEIGHT value="40" />,' oasisregtest-orig/maindoc.xml
perl -pi -e 's/<FLOW dir="L"/<FLOW/ ; s/relativetextsize="0.58" //' oasisregtest-final/maindoc.xml
perl -pi -e 's/modificationDate=\"[0-9-T:]*\"//; $_="" if (/<COLOR blue=\"0\" red=\"0\" green=\"0\" \/>/);' oasisregtest-final/maindoc.xml oasisregtest-orig/maindoc.xml

# Some generic fixups: documentinfo was updated by the automated 'editing'
perl -pi -e '$_ = "" if (/<editing-cycles>/ || /<date>/ || /<full-name>/ || /<company>/ || /<email>/ )' oasisregtest-final/documentinfo.xml oasisregtest-orig/documentinfo.xml

# Compare initial and final "native format" files
diff -urp oasisregtest-orig oasisregtest-final 2>&1 | tee oasisdiff | less

echo "See oasisregtest-oasis for the OASIS xml files."
echo "For a better diffing mechanism, launch xemacs and paste into a terminal:"
echo "gnudoit '(ediff-files \"$PWD/oasisregtest-orig/maindoc.xml\" \"$PWD/oasisregtest-final/maindoc.xml\")'"
