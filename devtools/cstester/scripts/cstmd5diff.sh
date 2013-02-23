#!/bin/bash
#
# script to create input file for visualimagecomapre.
#
# The script goes over all documents in the documents dir and checks the
# previous and current result dir's md5 sums.  If it finds differences it
# creates a line with the documents name and the pages that are differenet.
# The script can be used together with the cstrunner.

if [ -z "$1" -o -z "$2" -o -z "$3" ]
then
   echo "usage: $0 <documents dir> <previous result dir> <current result dir>"
   exit 1
fi

if [ ! -d $1 ]
then
   echo "dir $1 does not exist"
   exit 1
fi

if [ ! -d $2 ]
then
   echo "dir $2 does not exist"
   exit 1
fi

if [ ! -d $3 ]
then
   echo "dir $3 does not exist"
   exit 1
fi

for file in ${1}/*
do
    md5_1="${2}/${file}.check/md5.txt"
    md5_2="${3}/${file}.check/md5.txt"
    if [ ! -e "${md5_1}" -o  ! -e "${md5_2}" ]
    then
        echo $file
    else
        if [ "$(md5sum $md5_1 | awk '{print $1}')" != "$(md5sum $md5_2 | awk '{print $1}')" ]
        then
            pages=$(join ${md5_1} ${md5_2} | awk '{if ($2 != $3) {printf("%s ", $1)}}')
            echo $file $pages
        fi
    fi
done
