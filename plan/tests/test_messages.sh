#! /bin/sh
# test_messages:
# $1: base directory

if test -z "$1"; then
    echo "FAIL Called without base directory"
    exit 1
fi
if ! test -d "$1"; then
    echo "$1 is not a directory"
    exit 1
fi

result=0
files=0
errors=0

# handle whitespace and some other special characters in dir/file names
while IFS= read -rd $'\0' file; do
    files=$((files + 1))
    echo "Testing $files: $file"
    echo `grep '^potfilename=' "$file"`
    count=`grep -c '^potfilename=' "$file"`
    if [[ $count != 1 ]]; then
        errors=$((errors + 1))
        echo "FAIL Did not find 'potfilename=' correct number of times"
        echo "        Found: $count"
        echo "     Expected: 1"
        result=2;
    fi
    echo "----------------------------------------------------------"
done < <(find "$1" -name 'Messages.sh' -type f -print0 )

echo
echo "Found and tested $files messagefiles, detected $errors errors"
echo
echo "----------------------------------------------------------"

exit $result
