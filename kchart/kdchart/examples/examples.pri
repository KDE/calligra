
# This file is included by all of the examples' *.pro files.

# Note: KDCHART_PATH is set in the calling .pro file,
#       before examples.pri is included



# 1. Work around a qmake bug:
#
#    The bug would produce invalid *.exe files when KD Chart is linked
#    statically and examples are build right after building the lib.
staticlib {
  CONFIG -= staticlib
  DEFINES += KDCHART_STATICLIB
}


# 2. Use the filename "kdchartd.dll" (or "kdchartd.lib") on Windows
#    to avoid name clashes between debug/non-debug versions of the
#    KD Chart library:
KDCHARTLIB = kdchart
CONFIG(debug, debug|release) {
    !unix: KDCHARTLIB = "kdchartd"
}


# 3. Adjust the paths and LIBS according to KDCHART_PATH.
#    Note: KDCHART_PATH is set in the calling .pro file,
#          before examples.pri is included
INCLUDEPATH += \
            $$KDCHART_PATH/include \
            $$KDCHART_PATH/examples/tools
DEPENDPATH += \
            $$KDCHART_PATH/include \
            $$KDCHART_PATH/examples/tools
LIBS        += -L$$KDCHART_PATH/lib -l$$KDCHARTLIB -ltesttools


# 4. Miscellaneous settings:
unix {
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  UI_DIR = .ui
}
!unix {
  MOC_DIR = _moc
  OBJECTS_DIR = _obj
  UI_DIR = _ui
}
