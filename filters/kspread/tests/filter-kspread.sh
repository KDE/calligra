#!/bin/bash

# This script helps finding out problems in filter of kspread
# by converting .ksp -> filter extension -> .ksp and comparing the initial and final .ksp files.
# We use the ksp format as a "dump" of the KSpread data, to check if everything is correct
# in memory, but the point is of course to ensure that the filter extension has all the information.

# To use this script, you need to pass the full path to an existing ksp file as argument.
# Don't use a relative path, dcopstart won't handle it
input="$1"


appname=kspread
oldextension=ksp

case "$2" in 
	gnumeric)
	newextension=gnumeric
	newmimetype=application/x-gnumeric
	;;
	openoffice)
	newextension=sxc
	newmimetype=application/vnd.sun.xml.calc
	;;
	*)
	printf "Usage: %s <kspread file name> {gnumeric|openoffice}\n" "$0"
	exit 1;
esac

test -f "$input" || { echo "No such file $input"; exit 1; }

# Load old native file
appid=`dcopstart $appname $input`
test -n "$appid" || { echo "Error starting $appname!"; exit 1; }
while `dcop $appid Document-0 isLoading` == "true"; do
    sleep 1;
done

# Save again (in case of changes in syntax etc.)
origfile=$PWD/regtest-initial.$oldextension
dcop $appid Document-0 saveAs $origfile || exit 1
test -f $origfile || exit 1

# Save to filter extension
tmpfile=$PWD/regtest.$newextension
dcop $appid Document-0 setOutputMimeType $newmimetype || exit 1
dcop $appid Document-0 saveAs $tmpfile || exit 1
test -f $tmpfile || exit 1

dcopquit $appid

# Load resulting filter file, convert to old native format
tmpnativefile=$PWD/regtest-final.$oldextension
appid=`dcopstart $appname $tmpfile`
while `dcop $appid Document-0 isLoading` == "true"; do
    sleep 1;
done
dcop $appid Document-0 setOutputMimeType "application/x-$appname" || exit 1
dcop $appid Document-0 saveAs $tmpnativefile || exit 1
test -f $tmpnativefile || exit 1

# Unpack everything
rm -rf regtest-orig
mkdir regtest-orig
rm -rf regtest-final
mkdir regtest-final

cd regtest-orig || exit 1
unzip $origfile || exit 1
cd ..

cd regtest-final || exit 1
unzip $tmpnativefile || exit 1
cd ..

# Compare initial and final "native format" files
diff -urp regtest-orig regtest-final 2>&1 | tee filterdiff | less

echo "For a better diffing mechanism, launch xemacs and paste into a terminal:"
echo "gnudoit '(ediff-files \"$PWD/regtest-orig/maindoc.xml\" \"$PWD/regtest-final/maindoc.xml\")'"
