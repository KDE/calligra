#! /bin/sh
# Messages.sh files must have one instance of the line with:
# 'potfilename=<potfile>.pot'
# potfilename= must be at the start of the line and without spaces.
# It must refer to one pot file only.
# Release scripts rely on this.
potfilename=calligrastage.pot

source ../kundo2_aware_xgettext.sh

$EXTRACTRC `find . -name "*.ui" -o -name "*.rc"` >> rc.cpp || exit 11
kundo2_aware_xgettext $potfilename `find . -name \*.h -o -name \*.cpp`
rm -f rc.cpp
