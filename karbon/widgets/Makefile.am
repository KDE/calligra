INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../render \
	-I$(srcdir)/../dialogs \
	$(all_includes)

noinst_LTLIBRARIES = \
	libkarbonwidgets.la

noinst_HEADERS = \
	vcolorslider.h \
	vreference.h \
	vstrokefillpreview.h \
	vtranslate.h \
	vstatebutton.h \
	tkfloatspinbox.h \
	tkunits.h \
	tkfloatspinboxaction.h

libkarbonwidgets_la_SOURCES = \
	dummy.cc \
	vcolorslider.cc \
	vreference.cc \
	vstrokefillpreview.cc \
	vtranslate.cc \
	vstatebutton.cc \
	tkfloatspinbox.cpp \
	tkunits.cpp \
	tkfloatspinboxaction.cpp

libkarbonwidgets_la_METASOURCES = \
	AUTO

DISTCLEANFILES = \
	dummy.cc

dummy.cc:
	echo > dummy.cc

