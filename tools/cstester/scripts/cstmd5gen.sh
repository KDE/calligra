#!/bin/bash
# create md5sums for directory in parameter

if [ ! "$#" = "1" ]
then
    echo "Usage: $(basename $0) <dir>"
    exit 1
fi

if [ ! -d $1 ]
then
    echo "Directory \"$1\" does not exist!"
    exit 1
fi

md5sum $1/thumb_*.png | sed "s/ .*thumb_\([0-9]\+\).png/ \1/" | awk '{print $2, $1}' | sort -n > $1/md5.txt
