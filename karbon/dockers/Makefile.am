INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../commands \
	-I$(srcdir)/../tools \
	-I$(srcdir)/../widgets \
	-I$(srcdir)/../dialogs \
	$(all_includes)

noinst_LTLIBRARIES = \
	libkarbondockers.la

noinst_HEADERS = \
	vdocker.h \
	vtooldocker.h \
	vcolordocker.h \
	vcontexthelpdocker.h \
	vtooloptionsdocker.h

libkarbondockers_la_SOURCES = \
	dummy.cc \
	vdocker.cc \
	vtooldocker.cc \
	vcolordocker.cc \
	vcontexthelpdocker.cc \
	vtooloptionsdocker.cc

libkarbondockers_la_METASOURCES = AUTO

DISTCLEANFILES = \
	dummy.cc

dummy.cc:
	echo > dummy.cc

