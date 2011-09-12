#!/bin/bash
#
# script to check all documents in the current directory and create log of the failed ones .
# the logile will be written to LOG="../error-out.log" where out is replaced with the basename of the used out directory
LOG="../error-out.log"

if [ -z "$1" ]
then
   echo "usage $0 <outdir>"
   exit 1
fi 

if [ ! -d $1 ]
then
   LOG=${LOG/out/$(basename $1)};
   echo "outdir $1 does not exist"
   echo $LOG
   exit 1
fi

#clear LOG file
> $LOG
# make sure to stop processing of cstester to 60 seconds
ulimit -t 60
# make sure not to eat all available memory
ulimit -v 1269760

ls -b | grep -v "check$" | while read file
do
    if [ ! -e "${1}/${file}.check" ]
    then
        cstester --graphicssystem raster --outdir $1 --create "$file" || echo $file >> $LOG
    fi
done
