TEMPLATE = lib
TARGET = calligrawords

include($${TOP_SOURCE_DIR}/common.pri)

#QT += core gui xml sql network widgets
QT += core gui xml svg

#DEFINES += CALLIGRALIBS_LIBRARY

CALLIGRAWORDS_DIR = $${TOP_SOURCE_DIR}/../words

INCLUDEPATH += \
     $${CALLIGRAWORDS_DIR} \
     $${CALLIGRAWORDS_DIR}/part

LIBS += -lcalligralibs -lcalligraplugins

SOURCES += $$files($$CALLIGRAWORDS_DIR/*.cpp)
SOURCES += $$files($$CALLIGRAWORDS_DIR/part/*.cpp)
SOURCES += $$files($$CALLIGRAWORDS_DIR/part/author/*.cpp)
SOURCES += $$files($$CALLIGRAWORDS_DIR/part/commands/*.cpp)
SOURCES += $$files($$CALLIGRAWORDS_DIR/part/dialogs/*.cpp)
SOURCES += $$files($$CALLIGRAWORDS_DIR/part/dockers/*.cpp)
SOURCES += $$files($$CALLIGRAWORDS_DIR/part/frames/*.cpp)
SOURCES += $$files($$CALLIGRAWORDS_DIR/part/pagetool/*.cpp)
SOURCES -= $$files($$CALLIGRAWORDS_DIR/part/main.cpp)
SOURCES -= $$files($$CALLIGRAWORDS_DIR/part/author/main.cpp)
SOURCES -= $$files($$CALLIGRAWORDS_DIR/part/dockers/KWRdf*.cpp)

HEADERS += $$files($$CALLIGRAWORDS_DIR/*.h)
HEADERS += $$files($$CALLIGRAWORDS_DIR/part/*.h)
HEADERS += $$files($$CALLIGRAWORDS_DIR/part/author/*.h)
HEADERS += $$files($$CALLIGRAWORDS_DIR/part/commands/*.h)
HEADERS += $$files($$CALLIGRAWORDS_DIR/part/dialogs/*.h)
HEADERS += $$files($$CALLIGRAWORDS_DIR/part/dockers/*.h)
HEADERS += $$files($$CALLIGRAWORDS_DIR/part/frames/*.h)
HEADERS += $$files($$CALLIGRAWORDS_DIR/part/pagetool/*.h)
HEADERS -= $$files($$CALLIGRAWORDS_DIR/part/dockers/KWRdf*.h)

FORMS += $$files($$CALLIGRAWORDS_DIR/*.ui)
FORMS += $$files($$CALLIGRAWORDS_DIR/part/*.ui)
FORMS += $$files($$CALLIGRAWORDS_DIR/part/author/*.ui)
FORMS += $$files($$CALLIGRAWORDS_DIR/part/commands/*.ui)
FORMS += $$files($$CALLIGRAWORDS_DIR/part/dialogs/*.ui)
FORMS += $$files($$CALLIGRAWORDS_DIR/part/dockers/*.ui)
FORMS += $$files($$CALLIGRAWORDS_DIR/part/frames/*.ui)
FORMS += $$files($$CALLIGRAWORDS_DIR/part/pagetool/*.ui)

mocWrapper(HEADERS)
