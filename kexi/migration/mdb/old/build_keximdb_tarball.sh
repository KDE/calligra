#!/bin/sh
VERSION=1.1.0
DESTINATION=/tmp/keximdb-dist/$VERSION/

KDESVN=svn://anonsvn.kde.org/home/kde
CHECKOUT=keximdb-tmp

# From trunk/kdenonbeta
BRANCH=trunk/kdenonbeta

# From release branch
#BRANCH=branches/work/keximdb/koffice-1.6

#########
set -e
SVN="svn"
DATE=`date +%Y-%m-%d`

mkdir -p "$DESTINATION"
mkdir $CHECKOUT
pushd $CHECKOUT
  echo "1. Checking out top-level build files"
  $SVN co -N $KDESVN/$BRANCH keximdb
  cd keximdb

  echo "2. Checking out admin dir"
  $SVN co $KDESVN/branches/KDE/3.5/kde-common/admin

  echo "3. Checking out keximdb dir"
  $SVN up keximdb
  cd ..

  echo "4. Creating build system files"
  svn2dist --no-i18n keximdb keximdb -v $VERSION -b -g -m

  echo "5. Cleaning unneeded files"
  #Delete debian dir
  rm -r keximdb-$VERSION/keximdb/debian
  #Include debian dir
  #mv keximdb-$VERSION/keximdb/debian keximdb-$VERSION

  #Delete diffs dir
  rm -r keximdb-$VERSION/keximdb/src/diffs
  #Include diffs dir
  #mv keximdb-$VERSION/keximdb/src/diffs keximdb-$VERSION

  echo "6. Compiling LSM entry"
  sed "s/@DATE@/$DATE/;s/@VERSION@/$VERSION/" \
    ../keximdb.lsm > keximdb-$VERSION/keximdb.lsm

  printf "7. Finding size of tarball..."
  tar -cf keximdb-tmp.tar keximdb-$VERSION
  gzip -9 keximdb-tmp.tar
  SIZE=`du -k keximdb-tmp.tar.gz | awk '{print $1}'`
  echo  "$SIZE kB"
  sed -i "s/@SIZE@/$SIZE kB/" keximdb-$VERSION/keximdb.lsm
  rm keximdb-tmp.tar.gz

  echo "8. Creating tarball..."
  tar -cf keximdb-$VERSION.tar keximdb-$VERSION
  gzip -9 keximdb-$VERSION.tar
  
popd
