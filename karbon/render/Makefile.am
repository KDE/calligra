SUBDIRS = xrgbrender

INCLUDES = $(KOFFICE_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/xrgbrender \
	-I$(srcdir)/../core \
	$(LIBART_CFLAGS) \
	$(all_includes)

libvpainter_la_LIBADD = $(LIBART_LIBS) ./xrgbrender/libkarbonxrgbrender.la

noinst_LTLIBRARIES = libvpainter.la

noinst_HEADERS = \
	vpainter.h \
	vpainterfactory.h \
	vqpainter.h \
	art_render_pattern.h \
	art_rgb_svp.h \
	art_rgb.h \
	vkopainter.h

libvpainter_la_SOURCES = \
	vpainterfactory.cc \
	vqpainter.cc \
	art_render_pattern.c \
	art_rgb_svp.c \
	art_rgb.c \
	vkopainter.cc

libvpainter_la_METASOURCES = \
	AUTO
