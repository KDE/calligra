INCLUDES = $(KOFFICE_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../render \
	$(all_includes)

noinst_LTLIBRARIES = \
	libkarbonwidgets.la

noinst_HEADERS = \
	vcolorslider.h \
	vstrokefillpreview.h \
	vtoolcontainer.h \
	vtranslate.h \
	tkfloatspinbox.h \
	tkunits.h \
	tkfloatspinboxaction.h

libkarbonwidgets_la_SOURCES = \
	dummy.cc \
	vcolorslider.cc \
	vstrokefillpreview.cc \
	vtoolcontainer.cc \
	vtranslate.cc \
	tkfloatspinbox.cpp \
	tkunits.cpp \
	tkfloatspinboxaction.cpp

libkarbonwidgets_la_METASOURCES = \
	AUTO

DISTCLEANFILES = \
	dummy.cc

dummy.cc:
	echo > dummy.cc

