#! /bin/sh
# Messages.sh files must have one instance of the line with:
# 'potfilename=<potfile>.pot'
# potfilename= must be at the start of the line and without spaces.
# It must refer to one pot file only.
# Release scripts rely on this.
potfilename=braindump.pot

source ../kundo2_aware_xgettext.sh

$EXTRACTRC `find . -name \*.ui | grep -v '/tests/'` >> rc.cpp
RCFILES=`find . -name \*.rc`
$EXTRACTRC $RCFILES >> rc.cpp
$EXTRACTATTR --attr=category,name --attr=state,name data/states/states/*.xml >> rc.cpp

kundo2_aware_xgettext $potfilename rc.cpp `find . -name \*.cc -o -name \*.cpp -o -name \*.h`
rm -f rc.cpp
