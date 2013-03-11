TEMPLATE = app
TARGET = coffice

include($${TOP_SOURCE_DIR}/common.pri)

#QT += core gui xml sql network widgets
QT += core gui xml svg declarative

#DEFINES += CALLIGRALIBS_LIBRARY

CALLIGRAWORDS_DIR = $${TOP_SOURCE_DIR}/../words

INCLUDEPATH += \
     $${CALLIGRAWORDS_DIR} \
     $${CALLIGRAWORDS_DIR}/part

unix {
  !macx: LIBEXT = so
  macx: LIBEXT = dylib
  CONFIG( static ): LIBEXT = a
  LIBS += $${TOP_BUILD_DIR}/calligralibs/libcalligralibs.$${LIBEXT}
  LIBS += $${TOP_BUILD_DIR}/calligraplugins/libcalligraplugins.$${LIBEXT}
  LIBS += $${TOP_BUILD_DIR}/calligrawords/libcalligrawords.$${LIBEXT}
}
win32 {
    LIBS += -L$${TOP_BUILD_DIR}
    LIBS += -lcalligralibs
    LIBS += -lcalligraplugins
    LIBS += -lcalligrawords
}

# Add more folders to ship with the application, here
folder_01.source = qml/coffice
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
#QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
# CONFIG += qdeclarative-boostable

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    DocumentView.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

#INCLUDEPATH += $$PWD/../calligralibs
#DEPENDPATH += $$PWD/../calligralibs

HEADERS += \
    DocumentView.h

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
