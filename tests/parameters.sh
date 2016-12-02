#! /bin/sh

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
# copy from extract-messages.sh
XGETTEXT_FLAGS_WWW="\
--copyright-holder=This_file_is_part_of_KDE \
--msgid-bugs-address=http://bugs.kde.org \
--from-code=UTF-8 \
-L PHP \
-ki18n -ki18n_var -ki18n_noop \
"
