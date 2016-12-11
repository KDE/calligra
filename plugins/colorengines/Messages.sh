#! /bin/sh
source ../../kundo2_aware_xgettext.sh

kundo2_aware_xgettext kocolorspaces.pot `find . -name \*.cpp -o -name \*.h`
