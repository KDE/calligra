TEMPLATE = lib
TARGET = calligrafilters

include($${TOP_SOURCE_DIR}/common.pri)

#QT += core gui xml sql network widgets
#QT += core gui xml svg
QT += core gui xml

#CALLIGRAFILTERS_LIBMSO_DIR = $${TOP_SOURCE_DIR}/../filters/libmso/
#CALLIGRAFILTERS_MSWORD_DIR = $${TOP_SOURCE_DIR}/../filters/words/msword-odf/

CALLIGRAFILTERS_LIBMSOOXML_DIR = $${TOP_SOURCE_DIR}/../filters/libmsooxml/
CALLIGRAFILTERS_DOCX_DIR = $${TOP_SOURCE_DIR}/../filters/words/docx/

CALLIGRAFILTERS_XLSX_DIR = $${TOP_SOURCE_DIR}/../filters/sheets/xlsx/

INCLUDEPATH = \
     #$${CALLIGRAFILTERS_LIBMSO_DIR} \
     #$${CALLIGRAFILTERS_MSWORD_DIR} \
     $${CALLIGRAFILTERS_LIBMSOOXML_DIR} \
     $${CALLIGRAFILTERS_DOCX_DIR} \
     $${CALLIGRAFILTERS_XLSX_DIR} \
     $$INCLUDEPATH

LIBS += -lcalligralibs

# libmso
#SOURCES += $$files($$CALLIGRAFILTERS_LIBMSO_DIR/*.cpp)
#HEADERS += $$files($$CALLIGRAFILTERS_LIBMSO_DIR/*.h)
#SOURCES += $$files($$CALLIGRAFILTERS_LIBMSO_DIR/generated/*.cpp)
#HEADERS += $$files($$CALLIGRAFILTERS_LIBMSO_DIR/generated/*.h)

# doc
##SOURCES += $$files($$CALLIGRAFILTERS_MSWORD_DIR/*.cpp)
##HEADERS += $$files($$CALLIGRAFILTERS_MSWORD_DIR/*.h)
##SOURCES += $$files($$CALLIGRAFILTERS_MSWORD_DIR/wv2/src/*.cpp)
##HEADERS += $$files($$CALLIGRAFILTERS_MSWORD_DIR/wv2/src/*.h)
###SOURCES += $$files($$CALLIGRAFILTERS_MSWORD_DIR/wv2/src/generator/*.cpp)
###HEADERS += $$files($$CALLIGRAFILTERS_MSWORD_DIR/wv2/src/generator/*.h)

# libmsooxml
SOURCES += $$files($$CALLIGRAFILTERS_LIBMSOOXML_DIR/*.cpp)
HEADERS += $$files($$CALLIGRAFILTERS_LIBMSOOXML_DIR/*.h)

# docx
SOURCES += $$files($$CALLIGRAFILTERS_DOCX_DIR/*.cpp)
HEADERS += $$files($$CALLIGRAFILTERS_DOCX_DIR/*.h)

# for number formats
SOURCES += $$files($$CALLIGRAFILTERS_XLSX_DIR/NumberFormatParser.cpp)
HEADERS += $$files($$CALLIGRAFILTERS_XLSX_DIR/NumberFormatParser.h)

# for charting
SOURCES += $$files($$CALLIGRAFILTERS_XLSX_DIR/ChartExport.cpp)
HEADERS += $$files($$CALLIGRAFILTERS_XLSX_DIR/ChartExport.h)
HEADERS += $$files($$CALLIGRAFILTERS_XLSX_DIR/Charting.h)
SOURCES += $$files($$CALLIGRAFILTERS_XLSX_DIR/XlsxXmlChartReader.cpp)
HEADERS += $$files($$CALLIGRAFILTERS_XLSX_DIR/XlsxXmlChartReader.h)

mocWrapper(HEADERS)
