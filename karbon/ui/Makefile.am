INCLUDES = $(all_includes)

noinst_LTLIBRARIES = libkarbonui.la

noinst_HEADERS = \
	vcdlg_ellipse.h \
	vcdlg_polygon.h \
	vcdlg_rectangle.h \
	vcdlg_roundrect.h \
	vcdlg_sinus.h

libkarbonui_la_SOURCES = \
	dummy.cc \
	vcdlg_ellipse.cc \
	vcdlg_polygon.cc \
	vcdlg_rectangle.cc \
	vcdlg_roundrect.cc \
	vcdlg_sinus.cc

METASOURCES = AUTO

DISTCLEANFILES = \
	dummy.cc

dummy.cc:
	echo > dummy.cc

