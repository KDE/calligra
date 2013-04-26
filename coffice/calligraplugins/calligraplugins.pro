TEMPLATE = lib
TARGET = calligraplugins

include($${TOP_SOURCE_DIR}/common.pri)

#QT += core gui xml sql network widgets
QT += core gui xml svg

#DEFINES += CALLIGRAPLUGINS_LIBRARY

CALLIGRAPLUGINS_TEXTSHAPE_DIR = $${TOP_SOURCE_DIR}/../plugins/textshape
CALLIGRAPLUGINS_PICTURESHAPE_DIR = $${TOP_SOURCE_DIR}/../plugins/pictureshape
CALLIGRAPLUGINS_VARIABLES_DIR = $${TOP_SOURCE_DIR}/../plugins/variables
CALLIGRAPLUGINS_PATHSHAPE_DIR = $${TOP_SOURCE_DIR}/../plugins/pathshapes
CALLIGRAPLUGINS_VECTORIMAGE_DIR = $${TOP_SOURCE_DIR}/../libs/vectorimage
CALLIGRAPLUGINS_VECTORSHAPE_DIR = $${TOP_SOURCE_DIR}/../plugins/vectorshape
CALLIGRAPLUGINS_KDCHART_DIR = $${TOP_SOURCE_DIR}/../3rdparty/kdchart
CALLIGRAPLUGINS_CHARTSHAPE_DIR = $${TOP_SOURCE_DIR}/../plugins/chartshape

INCLUDEPATH = \
     $${TOP_SOURCE_DIR}/.. \
     $${CALLIGRAPLUGINS_TEXTSHAPE_DIR} \
     $${CALLIGRAPLUGINS_PICTURESHAPE_DIR} \
     $${CALLIGRAPLUGINS_VARIABLES_DIR} \
     $${CALLIGRAPLUGINS_PATHSHAPE_DIR} \
     $${CALLIGRAPLUGINS_VECTORIMAGE_DIR} \
     $${CALLIGRAPLUGINS_VECTORIMAGE_DIR}/libemf \
     $${CALLIGRAPLUGINS_VECTORIMAGE_DIR}/libsvm \
     $${CALLIGRAPLUGINS_VECTORIMAGE_DIR}/libwmf \
     $${CALLIGRAPLUGINS_VECTORSHAPE_DIR} \
     $${CALLIGRAPLUGINS_KDCHART_DIR}/kdablibfakes/include \
     $${CALLIGRAPLUGINS_KDCHART_DIR}/include/ \
     $${CALLIGRAPLUGINS_KDCHART_DIR}/src \
     $${CALLIGRAPLUGINS_KDCHART_DIR}/src/LeveyJennings/ \
     $${CALLIGRAPLUGINS_KDCHART_DIR}/src/PrerenderedElements/ \
     $${CALLIGRAPLUGINS_KDCHART_DIR}/src/Scenery/ \
     $${CALLIGRAPLUGINS_KDCHART_DIR}/src/Ternary/ \
     $${CALLIGRAPLUGINS_CHARTSHAPE_DIR} \
     $${CALLIGRAPLUGINS_CHARTSHAPE_DIR}/commands \
     $${CALLIGRAPLUGINS_CHARTSHAPE_DIR}/dialogs \
     $$INCLUDEPATH

LIBS += -lcalligralibs

#####################################################################
# TextShape

SOURCES += $$files($$CALLIGRAPLUGINS_TEXTSHAPE_DIR/TextPlugin.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_TEXTSHAPE_DIR/TextShapeFactory.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_TEXTSHAPE_DIR/TextShape.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_TEXTSHAPE_DIR/SimpleRootAreaProvider.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_TEXTSHAPE_DIR/ShrinkToFitShapeContainer.cpp)

HEADERS += $$files($$CALLIGRAPLUGINS_TEXTSHAPE_DIR/TextPlugin.h)
HEADERS += $$files($$CALLIGRAPLUGINS_TEXTSHAPE_DIR/TextShapeFactory.h)
HEADERS += $$files($$CALLIGRAPLUGINS_TEXTSHAPE_DIR/TextShape.h)
HEADERS += $$files($$CALLIGRAPLUGINS_TEXTSHAPE_DIR/SimpleRootAreaProvider.h)
HEADERS += $$files($$CALLIGRAPLUGINS_TEXTSHAPE_DIR/ShrinkToFitShapeContainer.h)

#####################################################################
# PictureShape

SOURCES += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/Plugin.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/PictureShapeFactory.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/PictureShape.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/ClippingRect.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/filters/GreyscaleFilterEffect.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/filters/MonoFilterEffect.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/filters/WatermarkFilterEffect.cpp)

HEADERS += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/Plugin.h)
HEADERS += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/PictureShapeFactory.h)
HEADERS += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/PictureShape.h)
HEADERS += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/ClippingRect.h)
HEADERS += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/filters/GreyscaleFilterEffect.h)
HEADERS += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/filters/MonoFilterEffect.h)
HEADERS += $$files($$CALLIGRAPLUGINS_PICTURESHAPE_DIR/filters/WatermarkFilterEffect.h)

#####################################################################
# Variables

SOURCES += $$files($$CALLIGRAPLUGINS_VARIABLES_DIR/*.cpp)

HEADERS += $$files($$CALLIGRAPLUGINS_VARIABLES_DIR/*.h)

FORMS += $$files($$CALLIGRAPLUGINS_VARIABLES_DIR/*.ui)

#####################################################################
# PathShape

SOURCES += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/enhancedpath/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/rectangle/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/star/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/ellipse/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/spiral/*.cpp)

HEADERS += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/enhancedpath/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/rectangle/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/star/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/ellipse/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/spiral/*.h)

FORMS += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/enhancedpath/*.ui)
FORMS += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/rectangle/*.ui)
FORMS += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/star/*.ui)
FORMS += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/ellipse/*.ui)
FORMS += $$files($$CALLIGRAPLUGINS_PATHSHAPE_DIR/spiral/*.ui)

#####################################################################
# VectorShape

SOURCES += $$files($$CALLIGRAPLUGINS_VECTORIMAGE_DIR/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_VECTORIMAGE_DIR/libemf/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_VECTORIMAGE_DIR/libsvm/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_VECTORIMAGE_DIR/libwmf/*.cpp)

HEADERS += $$files($$CALLIGRAPLUGINS_VECTORIMAGE_DIR/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_VECTORIMAGE_DIR/libemf/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_VECTORIMAGE_DIR/libsvm/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_VECTORIMAGE_DIR/libwmf/*.h)

SOURCES += $$files($$CALLIGRAPLUGINS_VECTORSHAPE_DIR/*.cpp)
HEADERS += $$files($$CALLIGRAPLUGINS_VECTORSHAPE_DIR/*.h)

#####################################################################
# ChartShape

SOURCES += $$files($$CALLIGRAPLUGINS_KDCHART_DIR/src/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_KDCHART_DIR/src/LeveyJennings/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_KDCHART_DIR/src/PrerenderedElements/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_KDCHART_DIR/src/Scenery/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_KDCHART_DIR/src/Ternary/*.cpp)

HEADERS += $$files($$CALLIGRAPLUGINS_KDCHART_DIR/src/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_KDCHART_DIR/src/LeveyJennings/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_KDCHART_DIR/src/PrerenderedElements/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_KDCHART_DIR/src/Scenery/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_KDCHART_DIR/src/Ternary/*.h)

SOURCES += $$files($$CALLIGRAPLUGINS_CHARTSHAPE_DIR/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_CHARTSHAPE_DIR/commands/*.cpp)
SOURCES += $$files($$CALLIGRAPLUGINS_CHARTSHAPE_DIR/dialogs/*.cpp)

HEADERS += $$files($$CALLIGRAPLUGINS_CHARTSHAPE_DIR/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_CHARTSHAPE_DIR/commands/*.h)
HEADERS += $$files($$CALLIGRAPLUGINS_CHARTSHAPE_DIR/dialogs/*.h)

FORMS += $$files($$CALLIGRAPLUGINS_KDCHART_DIR/src/*.ui)
FORMS += $$files($$CALLIGRAPLUGINS_CHARTSHAPE_DIR/*.ui)
FORMS += $$files($$CALLIGRAPLUGINS_CHARTSHAPE_DIR/dialogs/*.ui)

mocWrapper(HEADERS)
