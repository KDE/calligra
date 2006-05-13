




########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/../core \
#	-I$(srcdir)/../commands \
#	-I$(srcdir)/../render \
#	-I$(srcdir)/../dialogs \
#	-I$(srcdir)/../dockers \
#	-I$(srcdir)/../tools \
#	$(all_includes)
#
#noinst_LTLIBRARIES = \
#	libkarbonwidgets.la
#
#noinst_HEADERS = \
#	vcanvas.h \
#	vcolorslider.h \
#	vgradientwidget.h \
#	vgradienttabwidget.h \
#	vreference.h \
#	vstrokefillpreview.h \
#	vtranslate.h \
#	vselecttoolbar.h \
#	vsmallpreview.h \
#	vstatebutton.h \
#	vtoolbox.h \
#	vtypebuttonbox.h \
#	vruler.h
#
#libkarbonwidgets_la_SOURCES = \
#	dummy.cc \
#	vcanvas.cc \
#	vcolorslider.cc \
#	vgradientwidget.cc \
#	vgradienttabwidget.cc \
#	vreference.cc \
#	vstrokefillpreview.cc \
#	vtranslate.cc \
#	vselecttoolbar.cc \
#	vsmallpreview.cc \
#	vstatebutton.cc \
#	vtoolbox.cc \
#	vtypebuttonbox.cc \
#	vruler.cc
#
#libkarbonwidgets_la_METASOURCES = \
#	AUTO
#
#DISTCLEANFILES = \
#	dummy.cc
#
#dummy.cc:
#	echo > dummy.cc
#
