INCLUDES = \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../ksvg/impl/libs/art \
	-I$(srcdir)/../../ksvg/impl/libs/xrgbrender \
	-I$(srcdir)/../core \
	-DLIBART_COMPILATION \
	$(all_includes)

libvpainter_la_LIBADD = -lksvgart -lksvgxrgbrender

noinst_LTLIBRARIES = libvpainter.la

noinst_HEADERS = \
	vpainter.h \
	vpainterfactory.h \
	vqpainter.h \
	vkopainter.h

libvpainter_la_SOURCES = \
	vpainterfactory.cc \
	vqpainter.cc \
	vkopainter.cc

libvpainter_la_METASOURCES = \
	AUTO
