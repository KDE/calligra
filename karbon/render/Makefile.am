SUBDIRS = xrgbrender

INCLUDES = $(KOFFICE_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(top_srcdir)/lib/kopainter/support \
	-I$(srcdir)/xrgbrender \
	-I$(srcdir)/../core \
	-DLIBART_COMPILATION \
	$(all_includes)

libvpainter_la_LIBADD = $(top_builddir)/lib/kopainter/support/libsupport.la ./xrgbrender/libkarbonxrgbrender.la

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
