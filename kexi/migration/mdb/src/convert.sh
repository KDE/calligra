#!/bin/bash
MDB_CVS_DIR=mdbtools.cvs
MDB_ORIG_DIR=mdbtools.old
MDB_ANSI_DIR=mdbtools.ansi

usage(){
  echo Usage:
  echo "  convert.sh co"
  echo "    Checkout mdbtools from cvs into $MDB_CVS_DIR."
  echo "    Press enter when prompted for password."
  echo "  convert.sh symlink"
  echo "    Create symlink farm in $MDB_ORIG_DIR pointing to"
  echo "    mdbtools CVS checkout at $MDB_CVS_DIR."
  echo "  convert.sh ansi"
  echo "    Make a copy of $MDB_ORIG_DIR in $MDB_ANSI_DIR where"
  echo "    all non-ANSI comments have been deleted or ansi-fied."
  echo "  convert.sh update"
  echo "    Update keximdb copy of mdbtools from $MDB_ANSI_DIR."
  echo "  convert.sh patch"
  echo "    Apply keximdb patches from diffs dir."
  echo "  convert.sh changes"
  echo "    Show files that need to be patched by diffs."
  exit 1
}

build_links(){
  rm -rf $MDB_ORIG_DIR
  mkdir -p $MDB_ORIG_DIR/include
  ln -s ../../$MDB_CVS_DIR/include/mdbtools.h $MDB_ORIG_DIR/include
  mkdir $MDB_ORIG_DIR/libmdb
  for d in  mdbtools/libmdb/*.c ; do 
    ln -s ../../$MDB_CVS_DIR/src/libmdb/`basename $d` $MDB_ORIG_DIR/libmdb
  done
  exit
}

build_ansi(){
  # ANSI comments or nothing
  # Delete //-style comments
  # so that they can be compiled with -ansi (used in the KDE
  # build system).  It just deletes the comments, in case they
  # have further /*'s inside them.
  rm -rf $MDB_ANSI_DIR
  cp -r $MDB_ORIG_DIR $MDB_ANSI_DIR
  for d in $MDB_ANSI_DIR/libmdb/*.c ; do
    #sed -i 's#^\([ \t]*\)//\(.*\)##' $d
    sed -i 's#//\(.*\)##' $d
  done
  exit
}

update(){
  for d in mdbtools/libmdb/*.c ; do
    cp $MDB_ANSI_DIR/libmdb/`basename $d` mdbtools/libmdb
  done
  cp $MDB_ANSI_DIR/include/mdbtools.h mdbtools/include
  exit
}

apply_patch(){
  for d in diffs/*.diff ; do
    patch -p0 < $d
  done
  exit

}

show_patched(){
  diff -ru mdbtools.ansi/ mdbtools/ | \
    grep -v Only.in.mdbtools | \
    grep ^---
}

cvs_checkout(){
  CVSROOT=:pserver:anonymous@mdbtools.cvs.sourceforge.net:/cvsroot/mdbtools
  if (( ! `grep -c mdbtools $HOME/.cvspass` )) ; then
    cvs -d$CVSROOT login
  fi
  cvs -z3 -d$CVSROOT co -d $MDB_CVS_DIR mdbtools
}

case "$1" in
  co) cvs_checkout ;; 
  symlink) build_links ;; 
  ansi) build_ansi ;; 
  update) update ;; 
  patch) apply_patch ;; 
  changes) show_patched ;; 
  *) usage
esac

#cp $1/include/mdbtools.h mdbtools/include
#cp $1/libmdb/*.c mdbtools/libmdb
# No backends
#rm -f mdbtools/libmdb/{backend,stats,kkd,props}.c
# No mdb_table_dump, it uses backends and not used anyway
#sed -i '/^void mdb_table_dump/,/^}/d' mdbtools/libmdb/table.c

