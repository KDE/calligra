INCLUDES = $(KOFFICE_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../commands \
	-I$(srcdir)/../tools \
	$(all_includes)

noinst_LTLIBRARIES = \
	libkarbonui.la

noinst_HEADERS = \
	vcdlg_ellipse.h \
	vcdlg_polygon.h \
	vcdlg_rectangle.h \
	vcdlg_roundrect.h \
	vcdlg_sinus.h \
	vcdlg_spiral.h \
	vcdlg_star.h \
	vcolorslider.h \
	vmdlg_insertknots.h \
	vmdlg_solidfill.h \
	vmdlg_stroke.h \
	vmpanel_color.h \
	vtoolcontainer.h

libkarbonui_la_SOURCES = \
	dummy.cc \
	vcdlg_ellipse.cc \
	vcdlg_polygon.cc \
	vcdlg_rectangle.cc \
	vcdlg_roundrect.cc \
	vcdlg_sinus.cc \
	vcdlg_spiral.cc \
	vcdlg_star.cc \
	vcolorslider.cc \
	vmdlg_insertknots.cc \
	vmdlg_solidfill.cc \
	vmdlg_stroke.cc \
	vmpanel_color.cc \
	vtoolcontainer.cc

libkarbonui_la_METASOURCES = \
	AUTO

DISTCLEANFILES = \
	dummy.cc

dummy.cc:
	echo > dummy.cc

