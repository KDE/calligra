TEMPLATE = subdirs

CONFIG += ordered

QMAKE_CACHE = "$${OUT_PWD}/.qmake.cache"
TOP_SOURCE_DIR = $${IN_PWD}
TOP_BUILD_DIR = $${OUT_PWD}
system('echo \"\" > $${QMAKE_CACHE}')
system('echo TOP_SOURCE_DIR=$${TOP_SOURCE_DIR} >> $${QMAKE_CACHE}')
system('echo TOP_BUILD_DIR=$${TOP_BUILD_DIR} >> $${QMAKE_CACHE}')

include(common.pri)

SUBDIRS += calligralibs
SUBDIRS += calligrafilters
SUBDIRS += calligraplugins
SUBDIRS += calligrawords
SUBDIRS += calligrasheets
SUBDIRS += coffice

android:OTHER_FILES += \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/res/values-de/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-es/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-nl/strings.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/values-it/strings.xml \
    android/res/values-el/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/values-fr/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/layout/splash.xml \
    android/version.xml \
    android/AndroidManifest.xml
