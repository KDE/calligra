#!android {
#    TEMPLATE = app
#    TARGET = coffice
#}

include($${TOP_SOURCE_DIR}/common.pri)

# Overwrite the in common.pri set destdir for Android since
# it seems to not proper find the library otherwise :/
android:DESTDIR = $${TOP_BUILD_DIR}/coffice

#QT += core gui xml sql network widgets
QT += core gui xml svg declarative

CALLIGRAWORDS_DIR = $${TOP_SOURCE_DIR}/../words

INCLUDEPATH += \
     $${CALLIGRAWORDS_DIR} \
     $${CALLIGRAWORDS_DIR}/part

#LIBS += -lcalligralibs -lcalligraplugins -lcalligrawords
LIBS += -lcalligralibs -lcalligrawords

unix:!macx {
    # needed for XInitThreads hack
    LIBS += $$QMAKE_LIBS_X11
}

# Additional import path used to resolve QML modules in Creator's code model
#QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
#MOBILITY +=

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
#CONFIG += qdeclarative-boostable

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    DocumentView.cpp \
    FileSystemModel.cpp \
    Settings.cpp \
    AppManager.cpp \
    Document.cpp \
    Document_p.cpp

HEADERS += \
    DocumentView.h \
    FileSystemModel.h \
    Settings.h \
    AppManager.h \
    Document.h \
    Document_p.h

!isEmpty($$SAILFISH) {
    message(SailfishOS build)
    SOURCES += sailfishapplication/sailfishapplication.cpp
    HEADERS += sailfishapplication/sailfishapplication.h
    desktop.files = ../sailfish/coffice.desktop
    qml.files = qml/sailfish/*.qml
    include(sailfishapplication/sailfishapplication.pri)
    OTHER_FILES = ../sailfish/coffice.yaml
} else {

    ## Install extraLibraries
    #x86 {
    #      extraLibraries.path = /libs/x86
    #} else: armeabi-v7a {
    #      extraLibraries.path = /libs/armeabi-v7a
    #} else {
    #      extraLibraries.path = /libs/armeabi
    #}
    #INSTALLS += external_libs

    # Add more folders to ship with the application, here
    folder_01.source = qml/coffice
    folder_01.target = qml
    DEPLOYMENTFOLDERS += folder_01

    # Please do not modify the following two lines. Required for deployment.
    include(qmlapplicationviewer/qmlapplicationviewer.pri)
    qtcAddDeployment()
}
