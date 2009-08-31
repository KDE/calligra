#! /bin/sh
rm -f tips.cpp
EXCLUDE="-path ./doc -o -path ./plugins/importwizard -o -path ./tests -o -path ./scriptingplugins -o -path ./plugins/scripting -o -path ./plugins/reports -o -path ./3rdparty -o -path ./shapes"
LIST="data/*.rc `find . \( \( $EXCLUDE \) -prune -o -name \*.ui \) -type f | grep -v -e '/\.'`"
if test -n "$LIST"; then
	$EXTRACTRC $LIST >> rc.cpp
fi
LIST=`find . \( \( $EXCLUDE \) -prune -o -name \*.h -o -name \*.cpp \) -type f | grep -v -e '/\.' -e kexidswelcome.cpp`
if test -n "$LIST"; then \
	$XGETTEXT $LIST -o $podir/kexi.pot
fi
rm -f tips.cpp

