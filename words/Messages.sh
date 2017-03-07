#! /bin/sh
# Messages.sh files must have one instance of the line with:
# 'potfilename=<potfile>.pot'
# potfilename= must be at the start of the line and without spaces.
# It must refer to one pot file only.
# Release scripts rely on this.
potfilename=calligrawords.pot

source ../kundo2_aware_xgettext.sh

# Collect also the app strings in the part catalog, only a few strings,
# no need for overhead of separate catalog for the app
$EXTRACTRC `find part app  -name \*.ui -o -name \*.rc` >> rc.cpp
kundo2_aware_xgettext $potfilename rc.cpp *.cpp `find part app  -name \*.cpp -o -name \*.cpp -o -name \*.h`
rm -f rc.cpp
