TEMPLATE = lib
TARGET = testtools
DESTDIR	= ../../lib
win32: DLLDESTDIR = ../../bin
DEFINES += KDCHART_BUILD_TESTTOOLS_LIB TESTTOOLS_MAKEDLL
DEPENDPATH += .
INCLUDEPATH += .

#test.target = test
#test.commands = 
#test.depends = $(DESTDIR)/$(TARGET)
#QMAKE_EXTRA_TARGETS += test

# Input
HEADERS += TableModel.h
SOURCES += TableModel.cpp

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

