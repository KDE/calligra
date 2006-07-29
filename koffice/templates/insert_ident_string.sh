#!/bin/bash
if [ $# -ne 2 ]; then
  echo "Usage: $0 template mimetype"
  exit 1
fi
file=$1
mime=$2
if [ ! -e "$file" ]; then
  echo "$file doesn't exist"
  exit 2
fi

# Remove documentinfo.xml
mv $file $file.gz && gunzip $file.gz && tar --delete documentinfo.xml -f $file
gzip -n $file && mv -f $file.gz $file || exit 1

tmpfile=$file.tmp
echo -e -n '\x01f\x8b\x8\x8\x0\x0\x0\x0\x0\x3' > $tmpfile
echo -n "KOffice $mime" >> $tmpfile
echo -e -n '\x04\x06\x00' >> $tmpfile
#outsize=`stat $tmpfile | grep 'Size:' | gawk '{print $2;}'`
#dd if=$file of=$tmpfile bs=1 skip=10 seek=$outsize
#cut -b 11- $file --output-delimiter="" >> $tmpfile
tail --bytes=+11 $file >> $tmpfile

echo "Checking ident string:"
file $tmpfile

#length=${#mime}
#echo "Checking size (new one should be $((2+$length)) bigger)"
#ls -l $file $tmpfile

echo "Checking contents:"
tar tvzf $tmpfile

echo "Now do this if you're ok with the result: mv $tmpfile $file"

