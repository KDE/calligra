#!/bin/bash
if ! kde4-config ; then
  echo 'kde4-config not found ! Aborting. You need a more recent KDE, or to fix your $PATH.'
  exit 1
fi
IFS=:

echo -n "KDE prefixes: "
kde4-config --prefix

# Check for the binary
found=0
exedirs=`kde4-config --path exe`
for dir in $exedirs; do
  if [ -f "$dir/words" ]; then
    echo "words found in $dir"
    found=1
  fi
done

if [ $found -eq 0 ]; then
    echo "ERROR: words not found - looked at $exedirs"
fi

mimelnks=`kde4-config --path mime`
# Relevant existing mimetypes
mimes=""

for dir in $mimelnks; do
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

IFS=:
foundmagicfile=0
for dir in $mimelnks; do
  magic=$dir/magic
  if [ -f "$magic" ]; then
    echo "$magic says: "
    grep 'Microsoft\\ Word' $magic
    foundmagicfile=1
  fi
done

if [ $foundmagicfile -eq 0 ]; then
    echo "ERROR: Magic file not found $magic"
fi

for dir in `kde4-config --path services`; do
  echo Services dir $dir
  for mime in $mimes; do
     grep "Import=.*$mime" $dir/*.desktop
  done
done

