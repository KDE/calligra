SOURCES	+= main.cpp \
	../lib/mdatabasereportengine.cpp
HEADERS	+= ../lib/mdatabasereportengine.h
unix {
	INCLUDEPATH = ../lib
	LIBS = -lkugar
	LIBS += ../lib
  	UI_DIR = .ui
  	MOC_DIR = .moc
  	OBJECTS_DIR = .obj
}
win32 {
	DEFINES	= WIN32 RELEASE	
	INCLUDEPATH = ../lib
	LIBS += ../lib/kugarqt.lib
#	RC_FILE = resource.rc
	debug:MOC_DIR = ./tmp/moc/debug_mt_shared
	debug:OBJECTS_DIR = ./tmp/obj/debug_mt_shared
	release:MOC_DIR = ./tmp/moc/release_mt_shared
	release:OBJECTS_DIR = ./tmp/obj/release_mt_shared
#	UI_DIR = .ui
}
FORMS	= mainform.ui
TEMPLATE	=app
CONFIG	+= qt warn_on thread release
LANGUAGE	= C++
