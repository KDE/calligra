#!/bin/sh

# Builds .kexi files from .sql files.
# Only .kexi file that is older than .sql file is recreated.
# ksqlite is needed on the PATH

PATH=$PATH:../3rdparty/kexisql3/src/.libs/
which ksqlite > /dev/null || exit 1

for f in `ls -1 *.kexi.sql` ; do
	kexi_file=`echo $f | sed -e "s/\.kexi\.sql/\.kexi/"`
	if test -f $kexi_file -a ! $kexi_file -ot $f ; then
		echo "Local $kexi_file is newer than $f - skipping it"
		continue
	fi
	rm -f $kexi_file
	echo -n "Creating $kexi_file ... "
	ksqlite $kexi_file < $f || exit 1
	echo "OK"
done
