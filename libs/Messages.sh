#! /bin/sh
# Messages.sh files must have one instance of the line with:
# 'potfilename=<potfile>.pot'
# potfilename= must be at the start of the line and without spaces.
# It must refer to one pot file only.
# Release scripts rely on this.
potfilename=calligra.pot

source ../kundo2_aware_xgettext.sh

LIST=`find . \( -name \*.ui -o -name \*.rc \) -type f -print | grep -v '/tests/' | grep -v -e '/\.'`
$EXTRACTRC $LIST >> rc.cpp
LIST=`find . \( -name \*.cpp -o -name \*.cc -o -name \*.h \) -type f -print | grep -v '/tests/' | grep -v -e '/\.'`
kundo2_aware_xgettext "$potfilename" $LIST
rm -f rc.cpp
