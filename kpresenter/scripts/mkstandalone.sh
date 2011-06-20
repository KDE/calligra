#!/bin/sh
# This script copies everything that is needed to run stage on a
# computer where KDE isn't installed.
# Relies on stage being installed on the local machine, in $KDEDIR
#
# The script copies everything into a directory. You just need to tar it up,
# transfer it (CD-ROM or via Internet), untar it, cd into it,
# and run the script "go.sh" with your presentation as argument.
# Debug output goes to stage.out to avoid cluttering the terminal
# with our junk :-)  Check it out in case of problems.
#
# (C) David Faure <faure@kde.org>

# Pass output dir as argument
if [ $# -ne 1 ]; then
  echo "Usage : $0 <outputdir>"
else
  out=$1
########### Data
  mkdir -p $out
  mkdir -p $out/share
  mkdir -p $out/share/apps
  cp -r $KDEDIR/share/apps/stage $out/share/apps/
  cp -r $KDEDIR/share/apps/calligra $out/share/apps/
  cp -r $KDEDIR/share/apps/kstyle $out/share/apps/
  mkdir -p $out/share/config
  cp $KDEDIR/share/config/kdebug.areas $out/share/config/
  cp ~/.kde/share/config/kdeglobals $out/share/config/
  cp ~/.kde/share/config/stagerc $out/share/config/
  ### cp $KDEDIR/share/icons $out/share/
  cp -r $KDEDIR/share/toolbar $out/share/
  mkdir -p $out/share/servicetypes
  cp $KDEDIR/share/servicetypes/calligrapart.desktop $out/share/servicetypes
  mkdir -p $out/share/applnk
  cp $KDEDIR/share/applnk/Calligra/stage.desktop $out/share/applnk/

########### Libraries (warning, using Linux specific ldd !)
  TMPFILE=/tmp/__stage_ldd.tmp
  mkdir -p $out/lib
  ldd $KDEDIR/bin/stage | sed -e 's/.*=> \([^ ]*\).*/\1/' > $TMPFILE
  # Add style libs
  echo $KDEDIR/lib/basicstyle.so >> $TMPFILE
  echo $KDEDIR/lib/kstepstyle.so >> $TMPFILE
  # Add image handler libs
  echo $KDEDIR/lib/kimg_jpeg.so >> $TMPFILE
  echo $KDEDIR/lib/kimg_tiff.so >> $TMPFILE
  # Add libs for embedded parts here

  cat $TMPFILE | while read libfile; do
    # ldd shows the fullpath to a link.
    # Extract the directory from the fullpath
    origdir=`dirname $libfile`
    libfile=`basename $libfile`
    while [ -L $origdir/$libfile ]; do
        # Follow the symlink and create the same in $out/lib
	pointsto=`readlink $origdir/$libfile`
	echo "ln -s $out/$libfile $pointsto"
	ln -s $pointsto $out/lib/$libfile
	libfile=$pointsto;
    done
    # Copy the library itself
    echo "cp $origdir/$libfile $out/lib"
    cp $origdir/$libfile $out/lib
    # Copy the .la file as well
    lafile=`echo $libfile | sed -e 's/\.so.*/\.la/'`
    if [ -f $origdir/$lafile ]; then cp $origdir/$lafile $out/lib ; fi
  done
  rm -f $TMPFILE

########### Binaries
  mkdir -p $out/bin
  cp $KDEDIR/bin/stage $out/bin/
  cp $KDEDIR/bin/dcopserver $out/bin/
  # Take kded as well - we could copy the ksycoca file but we would have
  # to install it in ~/.kde ...
  cp $KDEDIR/bin/kded $out/bin/

########### Startup script
  echo '#!/bin/sh' > $out/go.sh
  echo 'export KDEDIR=`pwd`' >> $out/go.sh
  echo 'export LD_LIBRARY_PATH=$KDEDIR/lib' >> $out/go.sh
  echo 'mkdir -p ~/.kde 2>/dev/null' >> $out/go.sh
  echo 'mkdir -p ~/.kde/share 2>/dev/null' >> $out/go.sh
  echo 'mkdir -p ~/.kde/share/config 2>/dev/null' >> $out/go.sh
  echo 'cp share/config/* ~/.kde/share/config' >> $out/go.sh
  echo 'bin/dcopserver 2>/dev/null' >> $out/go.sh
  echo 'sleep 1 ; bin/kded 2>/dev/null' >> $out/go.sh
  echo 'sleep 1 ; bin/stage $* 1>/dev/null 2>stage.out &' >> $out/go.sh
  chmod u+x $out/go.sh
fi

