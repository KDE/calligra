#!/bin/bash

cd libmdb
for i in `ls -1 *.c` ; do cd ..; diff -u ./libmdb.cvs/$i ./libmdb/$i > ../diffs/`echo $i | sed -e 's/\.c/\.diff/g'` ; cd libmdb; done

cd ..
diff -u ./include.cvs/mdbtools.h ./include/mdbtools.h > ../diffs/mdbtools.diff
