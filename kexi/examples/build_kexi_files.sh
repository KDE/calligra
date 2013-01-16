#!/bin/sh

# Builds .kexi files from .sql files from the current directory.
# Only .kexi file that is older than .sql file is recreated.
# sqlite3 is needed on the PATH

for f in `ls -1 *.kexi.sql` ; do
	./build_kexi_file.sh $f
done
