#! /bin/sh
source ../../../calligra_xgettext.sh

calligra_xgettext KarbonFilterEffects.pot `find . -name \*.cpp -o -name \*.h`
