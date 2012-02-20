#! /bin/sh
EXCLUDE="-path ./3rdparty -o -path ./doc -o -path ./examples -o -path ./tests -o -path ./plugins/relations -o -path ./main/printing -o -path ./main/status -o -path ./mobile -o -path ./tools -o -path ./webforms -o -path ./plugins/forms/widgets/webbrowser -o -path ./plugins/forms/widgets/mapbrowser"
LIST="`find . \( \( $EXCLUDE \) -prune -o -name \*.ui \) -type f | grep -v -e '/\.'`"
if test -n "$LIST"; then
	$EXTRACTRC $LIST >> rc.cpp
fi
LIST=`find . \( \( $EXCLUDE \) -prune -o -name \*.h -o -name \*.cpp \) -type f` #| grep -v -e '/\.' -e kexidswelcome.cpp
if test -n "$LIST"; then
	$XGETTEXT $LIST -o $podir/kexi.pot
fi

