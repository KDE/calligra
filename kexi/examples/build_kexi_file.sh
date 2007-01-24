#!/bin/sh

# Builds a single .kexi file from a .sql file specified as $1.
# The otuput kexi file name is specified in $2.
# Only .kexi file that is older than .sql file is recreated.
# ksqlite is needed on the PATH
#


KEXISQL3PATH=../3rdparty/kexisql3/src/.libs/
PATH=$PATH:$KEXISQL3PATH
LD_LIBRARY_PATH=$KEXISQL3PATH:$KEXISQL3PATH
which ksqlite > /dev/null || exit 1

[ $# -lt 1 ] && echo "Missing .kexi.sql filename." && exit 1

#kexi_file=`echo $1 | sed -e "s/\.kexi\.sql/\.kexi/"`
kexi_file=$2
if test -f $kexi_file -a ! $kexi_file -ot $1 ; then
	echo "Local $kexi_file is newer than $1 - skipping it"
	exit 0
fi
rm -f $kexi_file
echo "Creating $kexi_file ... "
ksqlite $kexi_file < $1 || exit 1
echo "OK"
