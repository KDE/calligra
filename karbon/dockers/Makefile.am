INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../commands \
	-I$(srcdir)/../render \
	-I$(srcdir)/../tools \
	-I$(srcdir)/../widgets \
	-I$(srcdir)/../dialogs \
	$(all_includes)

noinst_LTLIBRARIES = \
	libkarbondockers.la

noinst_HEADERS = \
	vdocker.h \
	vcolordocker.h \
	vlayersdocker.h \
	vcontexthelpdocker.h \
	vstrokedocker.h \
	vtooloptionsdocker.h

libkarbondockers_la_SOURCES = \
	dummy.cc \
	vdocker.cc \
	vcolordocker.cc \
	vlayersdocker.cc \
	vcontexthelpdocker.cc \
	vstrokedocker.cc \
	vtooloptionsdocker.cc

libkarbondockers_la_METASOURCES = AUTO

DISTCLEANFILES = \
	dummy.cc

dummy.cc:
	echo > dummy.cc

