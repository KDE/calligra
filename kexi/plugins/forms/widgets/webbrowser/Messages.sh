#! /bin/sh
source ../../../../../calligra_xgettext.sh

calligra_xgettext `find . -name \*.cpp` > $podir/kformdesigner_webbrowser.pot
