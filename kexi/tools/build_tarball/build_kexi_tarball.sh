#!/bin/sh
#
# This script prepares & uploads tarballs
# Usage within KDE svn tree.
# Based on cvs2pack
# Copyright 2002 Nikolas Zimmermann <wildfox@kde.org>
# Improved by Jaroslaw Staniek, 2004-2005
# License: GPL (http://www.gnu.org/)

DIRNAME=`pwd`/`dirname $0`
cd $DIRNAME

# General
MODULE=koffice							# CHANGE
PROJECT=kexi							# CHANGE
PROJECT_TITLE=Kexi							# CHANGE
MODULE_PATH=trunk/$MODULE
KDEADMIN_PATH=trunk/KDE/kde-common/admin
KEXIMDB_PATH=trunk/kdenonbeta/keximdb
#for versions within koffice:
#PROJECT_PATH=koffice/kexi
#for versions independent of koffice:
PROJECT_VER=0.9
PROJECT_PATH=branches/kexi/$PROJECT_VER
SVN2DIST_OPTIONS=--no-i18n #set --no-i18n for version being outside the trunk

EXC="3rdparty/uuid main/projectWizard main/filters plugins/kugar plugins/html plugins/importwizard filters interfaces scriptig"

# Uploading
UPLOAD_USER=user						# CHANGE
UPLOAD_PASS=pass						# CHANGE
UPLOAD_HOST=host						# CHANGE
DO_UPLOAD=0									# 1 = Yes; 0 = No

# SVN'ing
SVN_USER=staniek						# CHANGE
SVN_PASS=										# CHANGE
SVN_HOST=https://svn.kde.org/home/kde		# CHANGE

# Dist-settings
# set empty if this is snapshot
DIST_VER=`grep "# define KEXI_VERSION_STRING" ../../kexi_version.h | sed -e 's/.*\"\(.*\)\"/\1/;s/ //g;y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/;'`

# Pick kexi_stable.lsm or kexi.lsm as a LSM file template
echo $DIST_VER | grep -e "beta" -e "rc" > /dev/null && lsmsrcfile=kexi.lsm || lsmsrcfile=kexi_stable.lsm

fixAppSpecific()
{
	rm ../changes-* #remove koffice-specific changelog
	mv CHANGES ../
	echo "For complete list of authors see kexi/main/kexiaboutdata.h file or use \"Help->About Kexi\" menu command." > ../AUTHORS
}

# Paths
DESTINATION=/tmp/kexi-dist/$DIST_VER				# CHANGE TO A NON-EXISTING DIR, WHICH WILL BE CREATED LATER!!!
CONFIGURE_PREFIX=`kde-config --prefix`	# CHANGE
EXPECT_PROGRAM=`which expect`				# CHANGE
ED_LOCATION=/bin						# CHANGE
SVN_PROGRAM=`which svn`					# CHANGE
DATE_PROGRAM=`which date`						# CHANGE
PERL_PROGRAM=`which perl`				# CHANGE
SVN2DIST_PROGRAM=`which svn2dist`			# CHANGE
NCFTPPUT_PROGRAM=`which ncftpput`				# CHANGE
CONF= # CANGE OR LEAVE EMPTY IF NO SPECIAL configure.in.in should be copied

# --- end of configuration area ---

# Main program
if [ -z "$DIST_VER" ]; then
	today=`$DATE_PROGRAM +%d-%m-%y`
	snapshot="-snapshot"
else
	today=$DIST_VER
	snapshot=
fi

echo "*** Creating $PROJECT-$today$snaphshot ***"

if ! [ -e $DESTINATION ]; then
	echo "*** Setup-Mode: Creating..."
	echo "     -> $DESTINATION..."
	mkdir $DESTINATION
	echo "     -> $DESTINATION/source..."
	mkdir $DESTINATION/source
	echo "     -> $DESTINATION/archive..."
	mkdir $DESTINATION/archive
fi

if [ -e $DESTINATION ]; then
#	rm -f $DESTINATION/expect_script

#	touch $DESTINATION/expect_script
#	chmod +x $DESTINATION/expect_script

#	echo "#!$EXPECT_PROGRAM
#		  spawn $SVN_PROGRAM -d :pserver:$SVN_USER@$CVS_HOST:/home/kde login
#		  expect -re \"SVN password: \"
#		  send \"$SVN_PASS\\r\"
#		  expect eof" >> $DESTINATION/expect_script
	
	rm -f $DESTINATION/LOG
fi	

echo $DESTINATION/source/$MODULE
if ! [ -e $DESTINATION/source/$MODULE ]; then
	echo "*** Setup-Mode: Checking $MODULE/$PROJECT out from SVN..."
	cd $DESTINATION/source
	
#	$DESTINATION/expect_script >> $DESTINATION/LOG 2>&1

	echo "$SVN_PROGRAM co -N $SVN_HOST/$MODULE_PATH" >> $DESTINATION/LOG
	$SVN_PROGRAM co -N $SVN_HOST/$MODULE_PATH >> $DESTINATION/LOG 2>&1 || exit 1
	echo "$SVN_PROGRAM co $SVN_HOST/$PROJECT_PATH" >> $DESTINATION/LOG
	$SVN_PROGRAM co $SVN_HOST/$PROJECT_PATH >> $DESTINATION/LOG 2>&1 || exit 1
	echo "$SVN_PROGRAM co $SVN_HOST/$KDEADMIN_PATH" >> $DESTINATION/LOG
	$SVN_PROGRAM co $SVN_HOST/$KDEADMIN_PATH >> $DESTINATION/LOG 2>&1 || exit 1
	mv $PROJECT_VER/$MODULE/$PROJECT $MODULE/ || exit 1
	rm -rf $PROJECT_VER || exit 1
	
	cd $MODULE || exit 1
	
	# Hacks
	if [ "$MODULE" = "kdenonbeta" ]; then
		$PERL_PROGRAM -i -ne 'print unless /COMPILE/' Makefile.am.in >> $DESTINATION/LOG 2>&1
	fi

	ln -s ../admin admin
	cd ..
fi

MIGRATION=$DESTINATION/source/$MODULE/kexi/migration
if ! [ -e $MIGRATION/keximdb ]; then
	echo "*** Setup-Mode: Checking $MIGRATION/keximdb out from SVN..."
	cd $MIGRATION || exit 1
	$SVN_PROGRAM co $SVN_HOST/$KEXIMDB_PATH >> $DESTINATION/LOG 2>&1 || exit 1
fi

echo "1. Cleaning up..." 
cd $DESTINATION/source/$MODULE || exit 1
rm -fr acinclude.m4 aclocal.m4 Makefile.in Makefile libtool $PROJECT/config.h \
config.h.in stamp-h.in subdirs configure configure.in configure.files stamp-h \
inst-apps autom4te-2.5x.cache  autom4te.cache .autoconf_trace MakeVars.in \
Makefile.am Makefile.rules.in
# we don't need warnings about Image Magick:
rm -f configure.in.bot

find . -name \*~ | xargs rm -f

echo "2. Updating from SVN..." 
#$DESTINATION/expect_script >> $DESTINATION/LOG 2>&1
cd admin || exit 1
$SVN_PROGRAM up >> $DESTINATION/LOG 2>&1 || exit 1
cd ../kexi || exit 1
$SVN_PROGRAM up >> $DESTINATION/LOG 2>&1 || exit 1

# KexiMDB plugin:
cd migration/keximdb || exit 1
$SVN_PROGRAM up >> $DESTINATION/LOG 2>&1 || exit 1
# -tweak configure scripts:
cp $DIRNAME/keximdb.configure.in.in configure.in.in || exit 1
cp $DIRNAME/keximdb.configure.in.bot configure.in.bot || exit 1

cd $DESTINATION/archive || exit 1
rm -f *
#rm -f $DESTINATION/expect_script

echo "3. Makefile creation..."
export UNSERMAKE=""
if [ -n "$CONF" ]; then
	cp $CONF $DESTINATION/source/$MODULE
fi
cd $DESTINATION/source/$MODULE/$PROJECT
for dir in $EXC; do
	rm -rf $dir
done
#fix exectutable bits for sources:
find . -name \*.h -o -name \*.cpp -o -name \*.c -o -name \*.cc -o -name \*.1 | xargs chmod a-x

#other app-specific fixes:
fixAppSpecific

#--svn2dist will do this
#cd $DESTINATION/source/$MODULE
#make -f Makefile.cvs >> $DESTINATION/LOG 2>&1
#cd ..
cd $DESTINATION/source
echo "4. Building tarballs..." 
$SVN2DIST_PROGRAM $MODULE $PROJECT -v $today \
	--svn-root "$SVN_HOST/trunk" \
	$SVN2DIST_OPTIONS \
	--log "$DESTINATION/LOG.SVN2DIST" >> $DESTINATION/LOG 2>&1 || exit 1
cat "svn2dist-$PROJECT.log" >> $DESTINATION/LOG
rm -Rf $PROJECT-$today
rm -f $PROJECT-$today.tar
mv $PROJECT-$today.* ../archive
cd ../archive

#create .lsm file
lsmfile=$PROJECT"-"$DIST_VER".lsm"
echo "Begin4
Title:           $PROJECT_TITLE
Version:         "$DIST_VER"
Entered-date:    "`date +%Y-%m-%d` > $lsmfile
cat $DIRNAME/$lsmsrcfile >> $lsmfile

if [ -n "$snapshot" ] ; then
	mv $PROJECT-$today.tar.bz2 $PROJECT-$today$snapshot.tar.bz2
	mv $PROJECT-$today.tar.gz $PROJECT-$today$snapshot.tar.gz
fi

if [ $DO_UPLOAD -eq 1 ]; then
	echo "5. Uploading tarballs..." 
	$NCFTPPUT_PROGRAM -u $UPLOAD_USER -p $UPLOAD_PASS $UPLOAD_HOST $PROJECT $DESTINATION/archive/* >> $DESTINATION/LOG 2>&1
	cd ..
	echo "6. Done!"
	exit
fi

echo "5. Won't upload. Done!"
