#!/bin/sh
# Small, simple and stupid script.
# It grabs a local copy of kfeedbackwizard from anonsvn
# and refactors KFeedbackWizard to KexiFeedbackWizard.
# The files will be placed in the correct directory
# so one can immediately use them.
#
# Copyright(C) 2005 by Christian Nitschkowski <segfault_ii@web.de>

cd ../../3rdparty
svn checkout svn://anonsvn.kde.org/home/kde/trunk/playground/utils/kfeedbackwizard

cd kfeedbackwizard
rm -rf .svn
rm -rf po/.svn
rm -rf lib/.svn
cat configure.in.in | sed -e s/kfeedbackwizard/kexifeedbackwizard/ >configure.in.in
cat Makefile.am | sed -e "s/SUBDIRS = po lib src/SUBDIRS = po lib/" >Makefile.am
rm po/kfeedbackwizard.pot
rm -rf src
rm -rf templates
rm kfeedbackwizard.kdevelop
rm Doxyfile
rm INSTALL
rm NEWS
cd lib
for i in `ls -1 kfeedback*`; do
 cat ${i} | sed -e s/KFeedback/KexiFeedback/g | sed -e s/KFEEDBACK/KEXIFEEDBACK/g | sed -e s/kfeedback/kexifeedback/g >$(echo ${i} | sed -e s/kfeedback/kexifeedback/)
 rm ${i}
done
cat Makefile.am | sed -e s/kfeedback/kexifeedback/g >Makefile.am
cd ../..
mv kfeedbackwizard kexifeedbackwizard
