TEMPLATE = lib
TARGET = calligrasheets

include($${TOP_SOURCE_DIR}/common.pri)

QT += core gui xml svg

CALLIGRASHEETS_DIR = $${TOP_SOURCE_DIR}/../sheets
THEIRDPARTY_DIR = $${TOP_SOURCE_DIR}/../3rdparty

DEFINES += SHEETS_NO_PLUGINMODULES
DEFINES += QT_NO_SQL

#DEFINES += EIGEN_USE_LAPACKE
DEFINES += EIGEN2_SUPPORT
#$${TOP_SOURCE_DIR}/Eigen \

INCLUDEPATH = \
    $${THEIRDPARTY_DIR} \
    $$CALLIGRASHEETS_DIR \
    $$CALLIGRASHEETS_DIR/database \
    $$CALLIGRASHEETS_DIR/shape \
    $$CALLIGRASHEETS_DIR/dialogs \
    $$CALLIGRASHEETS_DIR/functions \
    $$CALLIGRASHEETS_DIR/part \
    $$CALLIGRASHEETS_DIR/part/dialogs \
    $$CALLIGRASHEETS_DIR/part/commands \
    $$CALLIGRASHEETS_DIR/ui \
    $$CALLIGRASHEETS_DIR/mdds \
    $$CALLIGRASHEETS_DIR/chart \
    $$CALLIGRASHEETS_DIR/interfaces \
    $$CALLIGRASHEETS_DIR/commands \
    $$INCLUDEPATH

LIBS += -lcalligralibs

SOURCES += $$files($$CALLIGRASHEETS_DIR/*.cpp)
SOURCES += $$files($$CALLIGRASHEETS_DIR/database/*.cpp)
SOURCES += $$files($$CALLIGRASHEETS_DIR/shape/*.cpp)
SOURCES += $$files($$CALLIGRASHEETS_DIR/dialogs/*.cpp)
SOURCES += $$files($$CALLIGRASHEETS_DIR/functions/*.cpp)
SOURCES += $$files($$CALLIGRASHEETS_DIR/part/*.cpp)
SOURCES += $$files($$CALLIGRASHEETS_DIR/part/dialogs/*.cpp)
SOURCES += $$files($$CALLIGRASHEETS_DIR/part/commands/*.cpp)
SOURCES += $$files($$CALLIGRASHEETS_DIR/ui/*.cpp)
SOURCES += $$files($$CALLIGRASHEETS_DIR/mdds/*.cpp)
SOURCES += $$files($$CALLIGRASHEETS_DIR/chart/*.cpp)
#SOURCES += $$files($$CALLIGRASHEETS_DIR/interfaces/*.cpp)
SOURCES += $$files($$CALLIGRASHEETS_DIR/commands/*.cpp)

HEADERS += $$files($$CALLIGRASHEETS_DIR/*.h)
HEADERS += $$files($$CALLIGRASHEETS_DIR/database/*.h)
HEADERS += $$files($$CALLIGRASHEETS_DIR/shape/*.h)
HEADERS += $$files($$CALLIGRASHEETS_DIR/dialogs/*.h)
HEADERS += $$files($$CALLIGRASHEETS_DIR/functions/*.h)
HEADERS += $$files($$CALLIGRASHEETS_DIR/part/*.h)
HEADERS += $$files($$CALLIGRASHEETS_DIR/part/dialogs/*.h)
HEADERS += $$files($$CALLIGRASHEETS_DIR/part/commands/*.h)
HEADERS += $$files($$CALLIGRASHEETS_DIR/ui/*.h)
HEADERS += $$files($$CALLIGRASHEETS_DIR/mdds/*.h)
HEADERS += $$files($$CALLIGRASHEETS_DIR/chart/*.h)
#HEADERS += $$files($$CALLIGRASHEETS_DIR/interfaces/*.h)
HEADERS += $$files($$CALLIGRASHEETS_DIR/commands/*.h)

FORMS += $$files($$CALLIGRASHEETS_DIR/dialogs/*.ui)
FORMS += $$files($$CALLIGRASHEETS_DIR/part/dialogs/*.ui)
FORMS += $$files($$CALLIGRASHEETS_DIR/chart/*.ui)

#SOURCES -= $$files($$CALLIGRASHEETS_DIR/interfaces/*Adaptor.cpp)
#HEADERS -= $$files($$CALLIGRASHEETS_DIR/interfaces/*Adaptor.h)
#SOURCES -= $$files($$CALLIGRASHEETS_DIR/part/dialogs/DatabaseDialog.cpp)
#HEADERS -= $$files($$CALLIGRASHEETS_DIR/part/dialogs/DatabaseDialog.h)

EXTRA_MOC_HEADERS += $$files($$CALLIGRASHEETS_DIR/functions/*Module.h)
EXTRA_MOC_HEADERS += $$files($$CALLIGRASHEETS_DIR/database/DatabaseStorage.h)
EXTRA_MOC_HEADERS += $$files($$CALLIGRASHEETS_DIR/BindingModel.h)
EXTRA_MOC_HEADERS += $$files($$CALLIGRASHEETS_DIR/BindingStorage.h)
EXTRA_MOC_HEADERS += $$files($$CALLIGRASHEETS_DIR/ConditionsStorage.h)
EXTRA_MOC_HEADERS += $$files($$CALLIGRASHEETS_DIR/ValidityStorage.h)

mocWrapper(HEADERS, EXTRA_MOC_HEADERS)
