INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../widgets -I$(srcdir)/../dialogs  \
$(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES)

noinst_LTLIBRARIES = libkistools.la
# libkistools_a_LDFLAGS = $(all_libraries) -version-info 1:0
#libkistools_a_LIBADD =

libkistools_la_SOURCES = movetool.cc zoomtool.cc brushtool.cc gradienttool.cc

#libkistools_a_METASOURCES = 
#include_HEADERS =

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc *.h && mv messages.po ../../po/libkistools.pot
