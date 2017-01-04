#! /bin/sh
# test_messages:
# $1: base directory

if test -z $1; then
    echo "FAIL Called without base directory"
    exit 1
fi

result=0
messagefiles=`find $1 -name 'Messages.sh'`
files=0
errors=0
for messagefile in $messagefiles; do
    echo "Testing: $messagefile"
    files=$((files + 1))
    echo `grep '^potfilename=' $messagefile`
    count=`grep -c '^potfilename=' $messagefile`
    if [[ $count != 1 ]]; then
        errors=$((errors + 1))
        echo "FAIL Did not find 'potfilename=' correct number of times"
        echo "        Found: $count"
        echo "     Expected: 1"
        result=2;
    fi
    echo "----------------------------------------------------------"
done
echo
echo "Found and tested $files messagefiles, detected $errors errors"
echo
echo "----------------------------------------------------------"

exit $result
