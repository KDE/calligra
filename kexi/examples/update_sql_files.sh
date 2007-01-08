#!/bin/sh

# Updates .sql files using .kexi files
# Only .sql file that is older than .kexi file is recreated.
# ksqlite is needed on the PATH

KEXISQL3PATH=../3rdparty/kexisql3/src/.libs/
PATH=$PATH:$KEXISQL3PATH
LD_LIBRARY_PATH=$KEXISQL3PATH:$KEXISQL3PATH
which ksqlite > /dev/null || exit 1

for f in `ls -1 *.kexi` ; do
	if test -f $f.sql -a ! $f.sql -ot $f ; then
		echo "Local $f.sql is newer than $f - skipping it"
		continue
	fi
	echo -n "Creating $f.sql ... "
	echo "vacuum;" | ksqlite $f
	echo .dump | ksqlite $f > $f.sql || exit 1
	echo "OK"
done
