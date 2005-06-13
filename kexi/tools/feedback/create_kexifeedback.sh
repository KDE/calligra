#!/bin/sh
# Small, simple and stupid script.
# It grabs a local copy of kfeedbackwizard from anonsvn
# and refactors KFeedbackWizard to KexiFeedbackWizard.
# The files will be placed in the correct directory
# so one can immediately use them.
#
# Copyright(C) 2005 by Christian Nitschkowski <segfault_ii@web.de>

cd ../../3rdparty
[ -d kexifeedbackwizard ] && echo "kexifeedbackwizard/ already exists: giving up" && exit 0

echo Fetching kfeedbackwizard from anonsvn...

svn checkout svn://anonsvn.kde.org/home/kde/trunk/playground/utils/kfeedbackwizard

echo Refactoring KFeedbackWizard to KexiFeedbackWizard...

cd kfeedbackwizard
rm -rf .svn
rm -rf po/.svn
rm -rf lib/.svn
cat configure.in.in | sed -e s/kfeedbackwizard/kexifeedbackwizard/ >configure.in.in.tmp
mv configure.in.in.tmp configure.in.in
cat Makefile.am | sed -e "s/SUBDIRS = po lib src/SUBDIRS = po lib/" >Makefile.am.tmp
mv Makefile.am.tmp Makefile.am
rm po/kfeedbackwizard.pot
rm -rf src
rm -rf templates
rm kfeedbackwizard.kdevelop
rm Doxyfile
rm INSTALL
rm NEWS
cd lib
for i in `ls -1 kfeedback*`; do
 cat ${i} | sed -e "s/KFeedback/KexiFeedback/g;s/KFEEDBACK/KEXIFEEDBACK/g;s/kfeedback/kexifeedback/g" \
  >$(echo ${i} | sed -e s/kfeedback/kexifeedback/)
 rm ${i}
done
cat Makefile.am | sed -e s/kfeedback/kexifeedback/g >Makefile.am.tmp
mv Makefile.am.tmp Makefile.am
cd ../..
mv kfeedbackwizard kexifeedbackwizard

# Test if Makefile.am already contains a reference to the feedbackdir
# If it's already there, stop here
cat Makefile.am | grep FEEDBACKDIR && echo Done && cd ../tools/feedback && exit 0
echo Adding kexifeedbackwizard to Makefile.am...
echo -e "if use_kexifeedback\nFEEDBACKDIR = kexifeedbackwizard\nendif\n" >Makefile.am.tmp
cat Makefile.am | sed -e "s/SUBDIRS = */SUBDIRS = \$(FEEDBACKDIR) /" >>Makefile.am.tmp
mv Makefile.am Makefile.am.nofw
mv Makefile.am.tmp Makefile.am
cd ../tools/feedback
echo Done
