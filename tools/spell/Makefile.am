
INCLUDES = -I$(srcdir)/../../lib/kofficecore -I$(idldir) $(all_includes)
# LDFLAGS	= $(all_libraries) $(KDE_RPATH)
libkspelltool_la_LIBADD = $(all_libraries) ../../lib/kofficecore/libkofficecore.la -lkspell

####### Files

lib_LTLIBRARIES = libkspelltool.la

libkspelltool_la_SOURCES = main.cc

libkspelltool_la_LDFLAGS = -avoid-version -module

noinst_HEADERS = main.h

libkspelltool_la_METASOURCES = main.moc

service_DATA = kspelltool.desktop
servicedir = $(kde_servicesdir)

messages:
#	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po $(podir)/kspelltool.pot

