include($${TOP_SOURCE_DIR}/common.pri)

TEMPLATE = lib
TARGET = calligralibs
#TARGET = libcalligralibs

#QT += core gui xml sql network widgets
QT += core gui xml svg

DEFINES += CALLIGRALIBS_LIBRARY
DEFINES += MAKE_FLAKE_LIB
DEFINES += MAKE_BASICFLAKES_LIB
DEFINES += MAKE_VECTORIMAGE_LIB
DEFINES += MAKE_KOODF_LIB
DEFINES += MAKE_KOPLUGIN_LIB
DEFINES += MAKE_KOTEXT_LIB
DEFINES += MAKE_TEXTLAYOUT_LIB
DEFINES += MAKE_KOWIDGETS_LIB
DEFINES += MAKE_KOWIDGETUTILS_LIB
DEFINES += MAKE_KUNDO2_LIB
DEFINES += MAKE_KOPAGEAPP_LIB
DEFINES += MAKE_KOMAIN_LIB

SOURCES += $$files($$CALLIGRALIBS_KDEFAKE/*.cpp)
SOURCES += $$files($$CALLIGRALIBS_KDEFAKE/kio/*.cpp)
SOURCES += $$files($$CALLIGRALIBS_KDEFAKE/kparts/*.cpp)

SOURCES += $$files($$CALLIGRALIBS_KOTEXT_DIR/*.cpp)
SOURCES += $$files($$CALLIGRALIBS_KOTEXT_DIR/changetracker/*.cpp)
SOURCES += $$files($$CALLIGRALIBS_KOTEXT_DIR/commands/*.cpp)
#SOURCES += $$files($$CALLIGRALIBS_KOTEXT_DIR/kohyphen/*.cpp)
SOURCES += $$files($$CALLIGRALIBS_KOTEXT_DIR/opendocument/*.cpp)
SOURCES += $$files($$CALLIGRALIBS_KOTEXT_DIR/styles/*.cpp)
SOURCES -= $$files($$CALLIGRALIBS_KOTEXT_DIR/KoTextRdfCore.cpp)

SOURCES += $$files($$CALLIGRALIBS_TEXTLAYOUT_DIR/*.cpp)
SOURCES -= $$files($$CALLIGRALIBS_TEXTLAYOUT_DIR/KoPageProvider.cpp)
SOURCES -= $$files($$CALLIGRALIBS_TEXTLAYOUT_DIR/KoTextBlockPaintStrategyBase.cpp)

SOURCES += $$files($$CALLIGRALIBS_ODF_DIR/*.cpp)

SOURCES += $$files($$CALLIGRALIBS_FLAKE_DIR/*.cpp)
SOURCES += $$files($$CALLIGRALIBS_FLAKE_DIR/commands/*.cpp)
SOURCES += $$files($$CALLIGRALIBS_FLAKE_DIR/svg/*.cpp)
SOURCES += $$files($$CALLIGRALIBS_FLAKE_DIR/tools/*.cpp)

SOURCES += $$files($$CALLIGRALIBS_KOPLUGIN_DIR/*.cpp)

SOURCES += $$files($$CALLIGRALIBS_KUNDO2_DIR/*.cpp)

SOURCES += $$files($$CALLIGRALIBS_MAIN_DIR/*.cpp)
SOURCES += $$files($$CALLIGRALIBS_MAIN_DIR/config/*.cpp)
SOURCES -= $$files($$CALLIGRALIBS_MAIN_DIR/*Adaptor*.cpp)
#SOURCES -= $$files($$CALLIGRALIBS_MAIN_DIR/KoTemplate*.cpp)

SOURCES += $$files($$CALLIGRALIBS_WIDGETSUTILS_DIR/*.cpp)
SOURCES += $$files($$CALLIGRALIBS_WIDGETS_DIR/*.cpp)

HEADERS += calligralibs_global.h

HEADERS += $$files($$CALLIGRALIBS_KDEFAKE/*.h)
HEADERS += $$files($$CALLIGRALIBS_KDEFAKE/kio/*.h)
HEADERS += $$files($$CALLIGRALIBS_KDEFAKE/kparts/*.h)
#HEADERS += $$files($$CALLIGRALIBS_KDEFAKE/soprano/*.h)

HEADERS += $$files($$CALLIGRALIBS_KOTEXT_DIR/*.h)
HEADERS += $$files($$CALLIGRALIBS_KOTEXT_DIR/changetracker/*.h)
HEADERS += $$files($$CALLIGRALIBS_KOTEXT_DIR/commands/*.h)
#HEADERS += $$files($$CALLIGRALIBS_KOTEXT_DIR/kohyphen/*.h)
HEADERS += $$files($$CALLIGRALIBS_KOTEXT_DIR/opendocument/*.h)
HEADERS += $$files($$CALLIGRALIBS_KOTEXT_DIR/styles/*.h)
HEADERS -= $$files($$CALLIGRALIBS_KOTEXT_DIR/KoTextRdfCore.h)

HEADERS += $$files($$CALLIGRALIBS_TEXTLAYOUT_DIR/*.h)
HEADERS -= $$files($$CALLIGRALIBS_TEXTLAYOUT_DIR/KoPageProvider.h)
HEADERS -= $$files($$CALLIGRALIBS_TEXTLAYOUT_DIR/KoTextBlockPaintStrategyBase.h)

HEADERS += $$files($$CALLIGRALIBS_ODF_DIR/*.h)

HEADERS += $$files($$CALLIGRALIBS_FLAKE_DIR/*.h)
HEADERS += $$files($$CALLIGRALIBS_FLAKE_DIR/commands/*.h)
HEADERS += $$files($$CALLIGRALIBS_FLAKE_DIR/svg/*.h)
HEADERS += $$files($$CALLIGRALIBS_FLAKE_DIR/tools/*.h)

HEADERS += $$files($$CALLIGRALIBS_KOPLUGIN_DIR/*.h)

HEADERS += $$files($$CALLIGRALIBS_KUNDO2_DIR/*.h)

HEADERS += $$files($$CALLIGRALIBS_MAIN_DIR/*.h)
HEADERS += $$files($$CALLIGRALIBS_MAIN_DIR/config/*.h)
#HEADERS -= $$files($$CALLIGRALIBS_MAIN_DIR/KoTemplate*.h)

HEADERS += $$files($$CALLIGRALIBS_WIDGETSUTILS_DIR/*.h)
HEADERS += $$files($$CALLIGRALIBS_WIDGETS_DIR/*.h)

FORMS += $$files($$CALLIGRALIBS_FLAKE_DIR/*.ui)
FORMS += $$files($$CALLIGRALIBS_FLAKE_DIR/tools/*.ui)
FORMS += $$files($$CALLIGRALIBS_MAIN_DIR/*.ui)
FORMS += $$files($$CALLIGRALIBS_MAIN_DIR/config/*.ui)
FORMS += $$files($$CALLIGRALIBS_WIDGETS_DIR/*.ui)

#unix:!symbian {
#    maemo5 {
#        target.path = /opt/usr/lib
#    } else {
#        target.path = /usr/lib
#    }
#    INSTALLS += target
#}

#OTHER_FILES += \
#    android/src/org/kde/necessitas/origo/QtApplication.java \
#    android/src/org/kde/necessitas/origo/QtActivity.java \
#    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
#    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
#    android/res/values-de/strings.xml \
#    android/res/values-zh-rCN/strings.xml \
#    android/res/values-fa/strings.xml \
#    android/res/values-es/strings.xml \
#    android/res/values-nl/strings.xml \
#    android/res/values-it/strings.xml \
#    android/res/values-el/strings.xml \
#    android/res/values-fr/strings.xml \
#    android/res/values-pl/strings.xml \
#    android/res/values-ro/strings.xml \
#    android/res/values-nb/strings.xml \
#    android/res/values-et/strings.xml \
#    android/res/values-rs/strings.xml \
#    android/res/values-ja/strings.xml \
#    android/res/values-id/strings.xml \
#    android/res/values/strings.xml \
#    android/res/values-pt-rBR/strings.xml \
#    android/res/values-zh-rTW/strings.xml \
#    android/res/values-ru/strings.xml \
#    android/res/values-ms/strings.xml \
#    android/res/layout/splash.xml \
#    android/version.xml \
#    android/res/drawable-ldpi/icon.png \
#    android/res/drawable/icon.png \
#    android/res/drawable/logo.png \
#    android/res/drawable-hdpi/icon.png \
#    android/res/values/libs.xml \
#    android/res/drawable-mdpi/icon.png \
#    android/AndroidManifest.xml

mocWrapper(HEADERS)
