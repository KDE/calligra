#! /bin/sh
# test_kundo2_i18n:
# $1: calligra_xgettext.sh 
# $2: xgettext
# $3: msgcat
# $4: podir

# source the calligra_xgettext.sh script
. $1

# setup environment variables for calligra_xgettext.sh
XGETTEXT_PROGRAM=$2
MSGCAT=$3
podir=$4

# copy from extract-messages.sh
XGETTEXT_FLAGS="\
--copyright-holder=This_file_is_part_of_KDE \
--msgid-bugs-address=http://bugs.kde.org \
--from-code=UTF-8 \
-C --kde \
-ci18n \
-ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 \
-ki18nd:2 -ki18ndc:2c,3 -ki18ndp:2,3 -ki18ndcp:2c,3,4 \
-kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 \
-kki18nd:2 -kki18ndc:2c,3 -kki18ndp:2,3 -kki18ndcp:2c,3,4 \
-kxi18n:1 -kxi18nc:1c,2 -kxi18np:1,2 -kxi18ncp:1c,2,3 \
-kxi18nd:2 -kxi18ndc:2c,3 -kxi18ndp:2,3 -kxi18ndcp:2c,3,4 \
-kkxi18n:1 -kkxi18nc:1c,2 -kkxi18np:1,2 -kkxi18ncp:1c,2,3 \
-kkxi18nd:2 -kkxi18ndc:2c,3 -kkxi18ndp:2,3 -kkxi18ndcp:2c,3,4 \
-kI18N_NOOP:1 -kI18NC_NOOP:1c,2 \
-kI18N_NOOP2:1c,2 -kI18N_NOOP2_NOSTRIP:1c,2 \
-ktr2i18n:1 -ktr2xi18n:1 \
"

# calligra_xgettext.sh wants this in one variable
XGETTEXT="$XGETTEXT_PROGRAM $XGETTEXT_FLAGS"

potfile="test_kundo2_i18n.pot"
cppfile=test_kundo2_i18n.cpp

calligra_xgettext  $potfile $cppfile

# check result
if test ! -e $podir/$potfile; then
    echo "FAIL: pot file not created"
    exit 2
fi
if test 1 -ne `grep qtundo-format $podir/$potfile|wc -l`; then
    echo "FAIL: to many qtundo-format strings"
    exit 1
fi

exit 0
