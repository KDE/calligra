#!/bin/sh
#
# cvs2pack scripts, which creates & uploads tarballs
# Usage within KDE cvs tree.
# Copyright 2002 Nikolas Zimmermann <wildfox@kde.org>
# Improved by Jaroslaw Staniek, 2004
# License: GPL

DIRNAME=`dirname $0`

# General
MODULE=koffice							# CHANGE
PROJECT=kexi							# CHANGE

# Uploading
UPLOAD_USER=user						# CHANGE
UPLOAD_PASS=pass						# CHANGE
UPLOAD_HOST=host						# CHANGE
DO_UPLOAD=0								# 1 = Yes; 0 = No

# CVS'ing
CVS_USER=staniek						# CHANGE
CVS_PASS=								# CHANGE
CVS_HOST=cvs.kde.org				# CHANGE

# Dist-settings
TAG=0.1beta5						#CHANGE (or leave empty if snapshot)
EXC="3rdparty/uuid main/projectWizard main/filters plugins/kugar plugins/importwizard plugins/scripting filters interfaces migration"

# Paths
DESTINATION=/tmp/kexi-dist/$TAG				# CHANGE TO A NON-EXISTING DIR, WHICH WILL BE CREATED LATER!!!
CONFIGURE_PREFIX=`kde-config --prefix`	# CHANGE
EXPECT_PROGRAM=`which expect`				# CHANGE
ED_LOCATION=/bin						# CHANGE
CVS_PROGRAM=`which cvs`					# CHANGE
DATE_PROGRAM=`which date`						# CHANGE
PERL_PROGRAM=`which perl`				# CHANGE
CVS2DIST_PROGRAM=`which cvs2dist`			# CHANGE
NCFTPPUT_PROGRAM=`which ncftpput`				# CHANGE
CONF= #"/home/luci/kexi-admin/configure.in.in"		# CANGE OR LEAVE EMPTY IF NO SPECIAL configure.in.in should be copied

# Main program
if [ -z "$TAG" ]; then
	today=`$DATE_PROGRAM +%d-%m-%y`
	snapshot="-snapshot"
else
	today=$TAG
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
	rm -f $DESTINATION/expect_script

	touch $DESTINATION/expect_script
	chmod +x $DESTINATION/expect_script

	echo "#!$EXPECT_PROGRAM
		  spawn $CVS_PROGRAM -d :pserver:$CVS_USER@$CVS_HOST:/home/kde login
		  expect -re \"CVS password: \"
		  send \"$CVS_PASS\\r\"
		  expect eof" >> $DESTINATION/expect_script
	
	rm $DESTINATION/LOG -f
fi	

if ! [ -e $DESTINATION/source/$MODULE ]; then
	echo "*** Setup-Mode: Checking $MODULE/$PROJECT out from CVS..."
	cd $DESTINATION/source
	
#	$DESTINATION/expect_script >> $DESTINATION/LOG 2>&1
	
	$CVS_PROGRAM -d :pserver:$CVS_USER@$CVS_HOST:/home/kde co -l $MODULE >> $DESTINATION/LOG 2>&1
	$CVS_PROGRAM -d :pserver:$CVS_USER@$CVS_HOST:/home/kde co $MODULE/$PROJECT >> $DESTINATION/LOG 2>&1
	$CVS_PROGRAM -d :pserver:$CVS_USER@$CVS_HOST:/home/kde co kde-common/admin >> $DESTINATION/LOG 2>&1
	
	pwd
	cd $MODULE
	
	# Hacks
	if [ "$MODULE" = "kdenonbeta" ]; then
		$PERL_PROGRAM -i -ne 'print unless /COMPILE/' Makefile.am.in >> $DESTINATION/LOG 2>&1
	fi

	ln -s ../kde-common/admin/ admin

	cd ..
fi

echo "1. Cleaning up..." 
cd $DESTINATION/source/$MODULE
rm -fr acinclude.m4 aclocal.m4 Makefile.in Makefile libtool config.h config.h.in stamp-h.in subdirs configure configure.in configure.files stamp-h inst-apps autom4te-2.5x.cache  autom4te.cache .autoconf_trace MakeVars.in Makefile.am Makefile.rules.in
echo "2. Updating from CVS..." 
#$DESTINATION/expect_script >> $DESTINATION/LOG 2>&1
cd admin
$CVS_PROGRAM up >> $DESTINATION/LOG 2>&1
cd ../$PROJECT
$CVS_PROGRAM up >> $DESTINATION/LOG 2>&1
cd $DESTINATION/archive
rm -f *
rm -f $DESTINATION/expect_script
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
find . -name \*.h -o -name \*.cpp -o -name \*.c -o -name \*.cc | xargs chmod a-x
#--cvs2dist will do this
#cd $DESTINATION/source/$MODULE
#make -f Makefile.cvs >> $DESTINATION/LOG 2>&1
#cd ..
cd $DESTINATION/source
echo "4. Building tarballs..." 
$CVS2DIST_PROGRAM $MODULE $PROJECT -v $today >> $DESTINATION/LOG 2>&1
rm $PROJECT-$today -Rf
rm $PROJECT-$today.tar -f
mv $PROJECT-$today.* ../archive
cd ../archive

#create .lsm file
lsmfile="kexi-"$TAG".lsm"
echo "Begin4
Title:           Kexi
Version:         "$TAG"
Entered-date:    "`date +%Y-%m-%d` > $lsmfile
cat $DIRNAME/kexi.lsm >> $lsmfile

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
