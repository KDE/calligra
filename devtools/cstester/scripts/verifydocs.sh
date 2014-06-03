#!/bin/bash
#
# script to check documents against a previous run (with thumbnails) and log the pages of the documents that changed
# The output of this script can be read be visualimagecompare
# The logfile will be written to LOG="../verify-in-out.log" where in is replaced with the basename of the input directory
# and out by the output directory 
LOG="../verify-in-out.log"
OUT="../output.log"

if [ -z "$1" -o -z "$2" ]
then
   echo "usage $0 <indir> <outdir>"
   exit 1
fi 

if [ ! -d $1 ]
then
   echo "indir $1 does not exist"
   exit 1
fi

if [ ! -d $2 ]
then
   echo "outdir $2 does not exist"
   exit 1
fi

LOG=${LOG/in/$(basename $1)}
LOG=${LOG/out/$(basename $2)}

#clear LOG file
> $LOG
# make sure to stop processing of cstester to 60 seconds
ulimit -t 60
# make sure not to eat all available memory
ulimit -v 1269760

ls -b | grep -v "check$" | while read file
do
    if [ -e "${1}/${file}.check" ]
    then
        if ! cstester --graphicssystem raster --indir $1 --outdir $2 --verify "$file" 1>$OUT 2>&1
	then
	    echo -n "-"
            PAGES=$(grep "^Check failed:" ../output.log | sed "s/.*Page \([0-9]\+\) differ/\1/" | awk '{printf("%d ",$1)}')
            echo $file $PAGES>> $LOG
	else
	    echo -n "."
        fi
    fi
done
