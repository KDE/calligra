INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../core -I$(srcdir)/../ui -I$(srcdir)/../../lib/kofficeui $(all_includes)

noinst_LTLIBRARIES = libkarbontools.la

noinst_HEADERS = \
	vccmd_ellipse.h \
	vccmd_polygon.h \
	vccmd_rectangle.h \
	vccmd_roundrect.h \
	vccmd_sinus.h \
	vccmd_spiral.h \
	vccmd_star.h \
	vctool_rectangle.h \
	vctool_roundrect.h

libkarbontools_la_SOURCES = \
	vccmd_ellipse.cc \
	vccmd_polygon.cc \
	vccmd_rectangle.cc \
	vccmd_roundrect.cc \
	vccmd_sinus.cc \
	vccmd_spiral.cc \
	vccmd_star.cc \
	vctool_rectangle.cc \
	vctool_roundrect.cc

libkarbontools_la_LIBADD = \
        $(srcdir)/../ui/libkarbonui.la

libkarbontools_la_METASOURCES = \
	AUTO
