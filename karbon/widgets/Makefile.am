INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../commands \
	-I$(srcdir)/../render \
	-I$(srcdir)/../dialogs \
	-I$(srcdir)/../tools \
	$(all_includes)

noinst_LTLIBRARIES = \
	libkarbonwidgets.la

noinst_HEADERS = \
	vcolorslider.h \
	vcolordlg.h \
	vgradientwidget.h \
	vgradienttabwidget.h \
	vreference.h \
	vstrokefillpreview.h \
	vtranslate.h \
	vstatebutton.h \
	vtoolbox.h \
	tkfloatspinbox.h \
	tkunits.h \
	tkfloatspinboxaction.h

libkarbonwidgets_la_SOURCES = \
	dummy.cc \
	vcolorslider.cc \
	vcolordlg.cc \
	vgradientwidget.cc \
	vgradienttabwidget.cc \
	vreference.cc \
	vstrokefillpreview.cc \
	vtranslate.cc \
	vstatebutton.cc \
	vtoolbox.cc \
	tkfloatspinbox.cpp \
	tkunits.cpp \
	tkfloatspinboxaction.cpp

libkarbonwidgets_la_METASOURCES = \
	AUTO

DISTCLEANFILES = \
	dummy.cc

dummy.cc:
	echo > dummy.cc

