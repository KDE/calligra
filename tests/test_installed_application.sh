#! /bin/sh
# test_messages:
# $1: program
# $2: version
# $3: input file
# $4: data output directory

# check input parameters

prog=$(command -v "$1")

if ! echo "x_$prog" | grep -Fxq "x_$1"; then
    echo "FAIL  Cannot find correct program: $1, found: $prog"
    echo "Note: $1 must be installed to run this test."
    echo "      E.g: 'make install && ctest'"
    exit 1;
fi

if test -z "$3"; then
    echo "FAIL  Called without input file"
    exit 1
fi

if ! test -f "$3"; then
    echo "FAIL  $3 is not a file"
    exit 1
fi

if test -z "$4"; then
    echo "FAIL Called without data output directory"
    exit 1
fi
if ! test -d "$4"; then
    echo "FAIL $4 is not a directory"
    exit 1
fi


version=$("$prog" --version | cut -d ' ' -f 2)
echo "Testing: $prog version: $version"
if echo "$version" | grep -Fxq "$2"; then
    if ! $("$prog" --benchmark-loading '$3'); then
        echo "FAIL $prog failed"
        exit 1
    fi
else
    echo "FAIL Not correct version: $2"
    exit 1
fi

exit 0
