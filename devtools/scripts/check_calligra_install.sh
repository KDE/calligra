#!/bin/bash
if ! kde4-config ; then
  echo 'kde4-config not found ! Aborting. You need a more recent KDE, or to fix your $PATH.'
  exit 1
fi

instance=words
applnks=`kde4-config --path apps`:`kde4-config --path xdgdata-apps`
services=`kde4-config --path services`
found=0

function checkname()
{
  f=`echo $1 | sed -e 's,//,/,g'`
  if [ -f $f ]; then
    echo -n "Found: $f... "
    mimetypes=`grep ^MimeType $f|sed -e 's/.*=//;s/;/,/g;'`
    servicetypes=`grep ^ServiceTypes $f|sed -e 's/.*=//;s/$/,/g'`
    fulllist="$mimetypes,$servicetypes"
    if echo $fulllist | grep "Calligra/Part," >/dev/null 2>/dev/null; then
      echo "ok, this is a Calligra/Part"
    else
      echo '**** Calligra/Part not in list of types !'
      echo Service Types: $fulllist
      cat $f
      exit 1
    fi
    nativemime=`grep X-KDE-NativeMimeType $f`
    echo Native mimetype : $nativemime
    if echo $nativemime | grep "application/x-$instance" >/dev/null 2>/dev/null; then
      found=1
    fi
  fi
}

IFS=:
for dir in $services; do
  if test -n "$dir"; then
    echo Looking for ${instance}part.desktop under services dir: $dir
    f="$dir/${instance}part.desktop"
    checkname $f
  fi
done

if [ $found = 0 ]; then
  # The old way, as fallback
  for dir in $applnks; do
    if test -n "$dir"; then
      echo Looking under applnk dir: $dir
      f="$dir/Office/$instance.desktop"
      checkname $f
    fi
  done
fi

if [ $found = 0 ]; then
  echo
  echo Now looking by filename
  echo

  for dir in $applnks; do
    if test -n "$dir"; then
      echo Looking under applnk dir: $dir
      for f in `find $dir -name $instance.desktop`; do
        checkname $f
      done
    fi
  done
fi

if [ $found = 0 ]; then
  echo "***** No $instance.desktop with X-KDE-NativeMimetype in it was found! Installation problem."
  echo $applnks
fi

echo

stfound=0
for dir in `kde4-config --path servicetypes`; do
  echo Service Types dir $dir
  f=`echo $dir/calligra_part.desktop | sed -e 's,//,/,g'`
  if [ -f $f ]; then
    echo Found $f
    grep Native $f
    stfound=1
  fi
done

if [ $stfound = 0 ]; then
  echo '****** calligra_part.desktop not found!'
  kde4-config --path servicetypes
else
  echo
  if [ $found = 1 ]; then
    echo "Everything looks ok"
  fi
fi

