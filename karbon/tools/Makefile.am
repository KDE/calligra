INCLUDES = $(KOFFICE_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../visitors \
	-I$(srcdir)/../ui \
	-I$(srcdir)/../commands \
	-I$(srcdir)/../render \
	$(all_includes)

noinst_LTLIBRARIES = libkarbontools.la

noinst_HEADERS = \
	vctool_ellipse.h \
	vctool_polygon.h \
	vctool_rectangle.h \
	vctool_roundrect.h \
	vctool_sinus.h \
	vctool_spiral.h \
	vctool_star.h \
	vmtool_handle.h \
	vmtool_select.h \
	vmtool_rotate.h \
	vmtool_scale.h \
	vmtool_shear.h \
	vtool.h \
	vshapetool.h 

libkarbontools_la_SOURCES = \
	vctool_ellipse.cc \
	vctool_polygon.cc \
	vctool_rectangle.cc \
	vctool_roundrect.cc \
	vctool_sinus.cc \
	vctool_spiral.cc \
	vctool_star.cc \
	vmtool_handle.cc \
	vmtool_select.cc \
	vmtool_rotate.cc \
	vmtool_scale.cc \
	vmtool_shear.cc \
	vtool.cc \
	vshapetool.cc 

libkarbontools_la_LIBADD = \
	../ui/libkarbonui.la

libkarbontools_la_METASOURCES = \
	AUTO
