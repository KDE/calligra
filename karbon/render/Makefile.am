INCLUDES = $(KOFFICE_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../lib/kopainter/support \
	-I$(srcdir)/../../../kdenonbeta/ksvg/impl/libs/xrgbrender \
	-I$(srcdir)/../core \
	-DLIBART_COMPILATION \
	$(all_includes)

libvpainter_la_LIBADD = -lksvgxrgbrender ../../lib/kopainter/support/libsupport.la

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
