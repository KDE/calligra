#!/bin/bash

# This script helps finding out problems in filter of kspread/kword/kpresenter
# by converting .ksp/.kpr/.kwd -> filter extension -> .ksp/.kpr/.kwd and comparing the initial and final .ksp/.kpr/.kwd files.
# We use the ksp/kpr/kwd format as a "dump" of the KSpread data, to check if everything is correct
# in memory, but the point is of course to ensure that the filter extension has all the information.

# To use this script, you need to pass the full path to an existing ksp file as argument.
# Don't use a relative path, dcopstart won't handle it
input="$2"


case "$1" in 
	kspread)
	appname=kspread
	oldextension=ksp
	case "$3" in 
	    gnumeric)
	    newextension=gnumeric
	    newmimetype=application/x-gnumeric
	    ;;
	    opencalc)
	    newextension=sxc
	    newmimetype=application/vnd.sun.xml.calc
	    ;;
	    *)
	    printf "Usage: %s {kspread|kword|kpresenter} <file name> {gnumeric|opencalc}\n" "$0"
	    exit 1;
	esac
	;;
	kword)
	appname=kword
	oldextension=kwd
	case "$3" in 
	    rtf)
	    newextension=rtf
	    newmimetype=text/rtf
	    ;;
	    amipro)
	    newextension=sam
	    newmimetype=application/x-amipro
	    ;;
	    oowriter)
	    newextension=sxw
	    newmimetype=application/vnd.sun.xml.writer
	    ;;
	    latex)
	    newextension=tex
	    newmimetype=text/x-tex
	    ;;
	    paldoc)
	    newextension=pdb
	    newmimetype=application/vnd.palm
	    ;;
	    abiword)
	    newextension=abw
	    newmimetype=application/x-abiword
	    ;;
	    wml)
	    newextension=wml
	    newmimetype=application/text/vnd.wap.wml
	    ;;
	    wordperfect)
	    newextension=wpd
	    newmimetype=application/wordperfect
	    ;;
            ascii)
            newextension=txt
            newmimetype=text/plain
	    ;;
	    *)
	    printf "Usage: %s {kspread|kword|kpresenter} <file name> {rtf|amipro|oowriter|latex|paldoc|abiword|wml|wordperfect|ascii}\n" "$0"
	    exit 1;
	esac
	;;
	krita)
	appname=krita
	oldextension=kra
	case "$3" in
		magick-png)
		newextension=png
		newmimetype=image/png
		;;
                magick-gif)
                newextension=gif
                newmimetype=image/gif
                ;;
                magick-jpeg)
                newextension=jpg
                newmimetype=image/jpeg
                ;;
                magick-tiff)
                newextension=tiff
                newmimetype=image/tiff
                ;;
		*)
		printf "Usage: %s {kspread|kword|kpresenter|krita} <file name> {magick-png}\n" "$0"
		exit1;
	esac
	;;
	kpresenter)
	appname=kpresenter
	oldextension=kpr
	case "$3" in 
	    ooimpress)
	    newextension=sxi
	    newmimetype=application/vnd.sun.xml.impress
	    ;;
	    *)
	    printf "Usage: %s {kspread|kword|kpresenter} <file name> {ooimpress}\n" "$0"
	    exit 1;
	esac
	;;
	*)
	printf "Usage: %s {kspread|kword|kpresenter|krita} <file name> <type of filter>\n" "$0"
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
