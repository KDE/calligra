CONFIG += depend_includepath
CONFIG += qt warn_on

DEFINES += QT_FATAL_ASSERT
#DEFINES += USE_EXCEPTIONS
DEFINES += QT_NO_DBUS
#DEFINES += QT_NO_PRINTER

# Be less verbose
*-g++*|*-clang*|*-llvm* {
    QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-parameter -Wno-unused-function
}

# Increase the debugging level from Qt's default
#CONFIG(debug, debug|release) {
#    NORMAL_CXXFLAGS += -g3
#}

contains(TEMPLATE, lib) {
  DESTDIR = $${TOP_BUILD_DIR}/lib
}
contains(TEMPLATE, app) {
  DESTDIR = $${TOP_BUILD_DIR}/bin
}

#unix {
#    isEmpty(QMAKE_EXTENSION_SHLIB) {
#      static:QMAKE_EXTENSION_SHLIB=a
#      else {
#        macx:QMAKE_EXTENSION_SHLIB=dylib
#        else:QMAKE_EXTENSION_SHLIB=so
#      }
#    }
#}

# The calligra-libs code-paths are the common base for all other sub-projects
CALLIGRALIBS_KDEFAKE = $${TOP_SOURCE_DIR}/fake
CALLIGRALIBS_INTERFACES_DIR = $${TOP_SOURCE_DIR}/../interfaces
CALLIGRALIBS_ODF_DIR = $${TOP_SOURCE_DIR}/../libs/odf
CALLIGRALIBS_FLAKE_DIR = $${TOP_SOURCE_DIR}/../libs/flake
CALLIGRALIBS_KOPLUGIN_DIR = $${TOP_SOURCE_DIR}/../libs/koplugin
CALLIGRALIBS_KUNDO2_DIR = $${TOP_SOURCE_DIR}/../libs/kundo2
CALLIGRALIBS_MAIN_DIR = $${TOP_SOURCE_DIR}/../libs/main
CALLIGRALIBS_WIDGETSUTILS_DIR = $${TOP_SOURCE_DIR}/../libs/widgetutils
CALLIGRALIBS_WIDGETS_DIR = $${TOP_SOURCE_DIR}/../libs/widgets
CALLIGRALIBS_KOTEXT_DIR = $${TOP_SOURCE_DIR}/../libs/kotext
CALLIGRALIBS_TEXTLAYOUT_DIR = $${TOP_SOURCE_DIR}/../libs/textlayout

# Prepend the calligra-libs to the include-path
INCLUDEPATH = \
    $$CALLIGRALIBS_KDEFAKE \
    $${TOP_SOURCE_DIR}/calligralibs/ \
    $$CALLIGRALIBS_INTERFACES_DIR \
    $$CALLIGRALIBS_ODF_DIR \
    $$CALLIGRALIBS_FLAKE_DIR \
    $$CALLIGRALIBS_FLAKE_DIR/commands \
    $$CALLIGRALIBS_FLAKE_DIR/svg \
    $$CALLIGRALIBS_FLAKE_DIR/tools \
    $$CALLIGRALIBS_KOPLUGIN_DIR \
    $$CALLIGRALIBS_KUNDO2_DIR \
    $$CALLIGRALIBS_MAIN_DIR \
    $$CALLIGRALIBS_MAIN_DIR/config \
    $$CALLIGRALIBS_KOTEXT_DIR \
    $$CALLIGRALIBS_KOTEXT_DIR/changetracker \
    $$CALLIGRALIBS_KOTEXT_DIR/commands \
    $$CALLIGRALIBS_KOTEXT_DIR/kohyphen \
    $$CALLIGRALIBS_KOTEXT_DIR/opendocument \
    $$CALLIGRALIBS_KOTEXT_DIR/styles \
    $$CALLIGRALIBS_TEXTLAYOUT_DIR \
    $$CALLIGRALIBS_WIDGETSUTILS_DIR \
    $$CALLIGRALIBS_WIDGETS_DIR \
    $${TOP_BUILD_DIR} \
    $$INCLUDEPATH

# Set the directories where our own libraries are located
LIBS += -L$${TOP_BUILD_DIR}/lib

#new_moc.output = ${QMAKE_FILE_BASE}.moc
#new_moc.commands = moc ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
#new_moc.depend_command = g++ -E -M ${QMAKE_FILE_NAME} | sed "s,^.*: ,,"
#new_moc.input = HEADERS
#QMAKE_EXTRA_COMPILERS += new_moc

# CMake uses moc-files name "filename.moc" whereas qmake uses
# "moc_filename.cpp". Since calligra uses cmake we work around
# this mismatch by just creating moc wrapper files.
defineTest( mocWrapper ) {
    variable = $$1
    moc_headers = $$eval($$variable)
    for(hdr, moc_headers) {
        fdir=$$dirname(hdr)
        base=$$basename(hdr)
        fname=$$section(base, ".", 0, 0)
        in=$${LITERAL_HASH}include <moc_$${fname}.cpp>

        #out=$${OUT_PWD}/$${fname}.moc
        out=$${TOP_BUILD_DIR}/$${fname}.moc

        exists( $${fdir}/$${fname}.cpp ) {
            system(echo \"Creating moc-wrapper $$out\")

            # TODO Atm we just create a wrapper for all files but that's certainly not needed
            # and we could optimize by only creating wrappers for files that need them...
            #system(moc -E \"$$in\" | grep \"class\")

            system(echo \"$$in\" > \"$$out\")
        }
    }
}
