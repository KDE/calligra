#!/bin/sh

# Builds a single .kexi file from a .sql file specified as $1.
# The destination .kexi file is saved with name specified as $2.
# $2 can be omitted if $1 is of a form "name.kexi.sql" - then
# the destination file will be "name.kexi".
# Only .kexi file that is older than .sql file is recreated.
# ksqlite is needed on the $PATH.

KEXISQL3PATH=../3rdparty/kexisql3/src/.libs/
PATH=$KEXISQL3PATH:$PATH
export PATH
LD_LIBRARY_PATH=$KEXISQL3PATH:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
which ksqlite > /dev/null || exit 1

[ $# -lt 1 ] && echo "Missing .sql filename." && exit 1

if [ $# -lt 2 ] ; then
	kexi_file=`echo $1 | sed -e "s/\.kexi\.sql/\.kexi/"`
else
	kexi_file=$2
fi

if test -f "$kexi_file" -a ! "$kexi_file" -ot "$1" ; then
	echo "Local $kexi_file is newer than $1 - skipping it"
	exit 0
fi

rm -f "$kexi_file"
echo "Creating \"$kexi_file\" ... "
ksqlite "$kexi_file" < "$1" || exit 1
echo "OK"
