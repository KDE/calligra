#! /bin/sh
$EXTRACTRC lib/*.ui part/*.ui kudesigner/*.ui */*.rc > rc.cpp || exit 11
$XGETTEXT -ktranslate lib/*.cpp part/*.cpp kudesigner/*.cpp kudesigner_lib/*.cpp kudesigner_lib/*.h rc.cpp -o $podir/kugar.pot
