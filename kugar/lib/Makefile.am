INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)

METASOURCES = AUTO

libkugar_la_LIBADD = $(LIB_QT) $(LIB_KDEPRINT)

libkugar_la_LDFLAGS = $(all_libraries) -version-info 1:2 -no-undefined

lib_LTLIBRARIES = libkugar.la

libkugar_la_SOURCES = \
	mcalcobject.cpp \
	mfieldobject.cpp \
	mlabelobject.cpp \
	mlineobject.cpp \
	mpagecollection.cpp \
	mpagedisplay.cpp \
	mreportdetail.cpp \
	mreportengine.cpp \
	mreportobject.cpp \
	mreportsection.cpp \
	mreportviewer.cpp \
	kreportviewer.cpp \
	mspecialobject.cpp \
	mutil.cpp

include_HEADERS = \
	kugar.h \
	kugarqt.h \
	mcalcobject.h \
	mfieldobject.h \
	mlabelobject.h \
	mlineobject.h \
	mpagecollection.h \
	mpagedisplay.h \
	mreportdetail.h \
	mreportengine.h \
	mreportobject.h \
	mreportsection.h \
	mreportviewer.h \
	kreportviewer.h \
	mspecialobject.h \
	mutil.h

CLEANFILES = libkugar_la_meta_unload.cpp
