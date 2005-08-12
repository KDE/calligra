INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
	-I$(top_builddir)/kivio/kiviopart/config \
	-I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES =  libkiviotargettool.la
libkiviotargettool_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
libkiviotargettool_la_LIBADD = $(LIB_KOTEXT) $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la
libkiviotargettool_la_SOURCES = kiviotargettool.cpp kiviotargettoolfactory.cpp

noinst_HEADERS =  kiviotargettool.h kiviotargettoolfactory.h

rc_DATA = kiviotargettool.rc
rcdir = $(kde_datadir)/kivio/kpartplugins

desktop_DATA = kiviotargettool.desktop
desktopdir = $(kde_servicesdir)
