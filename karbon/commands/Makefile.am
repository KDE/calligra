INCLUDES = $(KOFFICE_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../visitors \
	$(all_includes)

noinst_LTLIBRARIES = libkarboncommands.la

noinst_HEADERS = \
	vccmd_ellipse.h \
	vccmd_polygon.h \
	vccmd_rectangle.h \
	vccmd_roundrect.h \
	vccmd_sinus.h \
	vccmd_spiral.h \
	vccmd_star.h \
	vccommand.h \
	vcommand.h \
	vmcmd_delete.h \
	vmcmd_fill.h \
	vmcmd_stroke.h \
	vmcmd_transform.h
	vmcmd_group.h

libkarboncommands_la_SOURCES = \
	vccmd_ellipse.cc \
	vccmd_polygon.cc \
	vccmd_rectangle.cc \
	vccmd_roundrect.cc \
	vccmd_sinus.cc \
	vccmd_spiral.cc \
	vccmd_star.cc \
	vccommand.cc \
	vcommand.cc \
	vmcmd_delete.cc \
	vmcmd_fill.cc \
	vmcmd_stroke.cc \
	vmcmd_transform.cc \
	vmcmd_group.cc

libkarboncommands_la_METASOURCES = \
	AUTO
