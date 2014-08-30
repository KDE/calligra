#! /usr/bin/env bash
source ../calligra_xgettext.sh

calligra_xgettext calligraactive.pot `find . -name \*.qml -o -name \*.cpp -o -name \*.h`
rm -f rc.cpp
