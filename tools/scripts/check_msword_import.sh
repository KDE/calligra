#!/bin/bash
if ! kde-config ; then
  echo 'kde-config not found ! Aborting. You need a more recent KDE, or to fix your $PATH.'
  exit 1
fi

mimelnks=`kde-config --path mime`
lastmimepath=""
found=0
# Relevant existing mimetypes
mimes=""

IFS=:
for dir in $mimelnks; do
  lastmimepath=$dir
  filename="application/msword.desktop"
  if [ -f "$dir$filename" ]; then
      echo -n "Found: $dir$filename... "
      hidden=`grep ^Hidden $dir$filename`
      if [ "$hidden" == "true" ]; then
          echo "deleted";
      else
          mimetype=`grep ^MimeType $dir$filename|sed -e 's/.*=//'`
          mimes="$mimes:$mimetype"; # using ':' because of IFS
          patterns=`grep ^Patterns $dir$filename|sed -e 's/.*=//'`
          if [ -n "$patterns" ]; then
              echo -n "(associated with $patterns)"
          fi
          echo
      fi
  fi
  filename="application/msword2.desktop"
  if [ -f "$dir$filename" ]; then
      echo -n "Found: $dir$filename... "
      hidden=`grep ^Hidden $dir$filename`
      if [ "$hidden" == "true" ]; then
          echo "deleted";
      else
          mimetype=`grep ^MimeType $dir$filename|sed -e 's/.*=//'`
          mimes="$mimes:$mimetype"; # using ':' because of IFS
          patterns=`grep ^Patterns $dir$filename|sed -e 's/.*=//'`
          if [ -n "$patterns" ]; then
              echo -n "(associated with $patterns)"
          fi
          echo
      fi
  fi
done

IFS=" "
mimes=`echo $mimes | sed -e 's/^://g'`
echo "Relevant mimetypes found: $mimes"

magic=$lastmimepath/magic
if [ -f "$magic" ]; then
    echo "magic file says: "
    grep 'Microsoft\\ Word' $magic
else
    echo "ERROR: Magic file not found $magic"
fi

IFS=:
for dir in `kde-config --path services`; do
  echo Services dir $dir
  for mime in $mimes; do
     grep "Import=.*$mime" $dir/*.desktop
  done
done
