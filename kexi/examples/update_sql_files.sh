#!/bin/sh

# Updates .sql files using .kexi files
# Only .sql file that is older than .kexi file is recreated.
# sqlite3 is needed on the PATH

which sqlite3 > /dev/null || exit 1

for f in `ls -1 *.kexi` ; do
	if test -f $f.sql -a ! $f.sql -ot $f ; then
		echo "Local $f.sql is newer than $f - skipping it"
		continue
	fi
	echo -n "Creating $f.sql ... "
	echo "vacuum;" | sqlite3 $f
	echo .dump | sqlite3 $f > $f.sql || exit 1
	echo "OK"
done
