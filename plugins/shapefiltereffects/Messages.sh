#! /bin/sh
source ../../kundo2_aware_xgettext.sh

kundo2_aware_xgettext KarbonFilterEffects.pot `find . -name \*.cpp -o -name \*.h`
