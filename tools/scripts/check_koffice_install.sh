#!/bin/bash
if ! kde-config ; then
  echo 'kde-config not found ! Aborting. You need a more recent KDE, or to fix your $PATH.'
  exit 1
fi

instance=kword
applnks=`kde-config --path apps`
found=0

function checkname()
{
  f=`echo $1 | sed -e 's,//,/,g'`
  if [ -f $f ]; then
    echo -n "Found: $f... "
    mimetypes=`grep ^MimeType $f|sed -e 's/.*=//;s/;/,/g;'`
    servicetypes=`grep ^ServiceTypes $f|sed -e 's/.*=//;s/$/,/g'`
    fulllist="$mimetypes,$servicetypes"
    if echo $fulllist | grep -q "KOfficePart,"; then
      echo "ok, this is a KOfficePart"
    else
      echo '**** KOfficePart not in list of types !'
      echo Service Types: $fulllist
      cat $f
      exit 1
    fi
    nativemime=`grep X-KDE-NativeMimeType $f`
    echo Native mimetype : $nativemime
    if echo $nativemime | grep -q "application/x-$instance"; then
      found=1
    fi
  fi
}

IFS=:
for dir in $applnks; do
  f="$dir/Office/$instance.desktop"
  checkname $f
done

if [ $found = 0 ]; then
  echo
  echo Now looking by filename
  echo

  for dir in $applnks; do
    echo Looking under applnk dir: $dir
    for f in `find $dir -name $instance.desktop`; do
      checkname $f
    done
  done
fi

if [ $found = 0 ]; then
  echo '***** Native mimetype not found !'
  echo $applnks
fi

echo

stfound=0
for dir in `kde-config --path servicetypes`; do
  echo Service Types dir $dir
  f=`echo $dir/kofficepart.desktop | sed -e 's,//,/,g'`
  if [ -f $f ]; then
    echo Found $f
    grep Native $f
    stfound=1
  fi
done

if [ $stfound = 0 ]; then
  echo '****** kofficepart.desktop not found!'
  kde-config --path servicetypes
else
  echo
  if [ $found = 1 ]; then
    echo "Everything looks ok"
  fi
fi

