AM_CPPFLAGS = -I$(top_srcdir)/kword $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = libwriteup_part.la
libwriteup_part_la_LIBADD = ../kword/libkwordprivate.la
libwriteup_part_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
libwriteup_part_la_SOURCES = WUDocument.cpp WUFactory.cpp WUView.cpp

METASOURCES = AUTO

## The kdeinit loadable module and the writeup binary
kdeinit_LTLIBRARIES = writeup.la
lib_LTLIBRARIES =
bin_PROGRAMS =
writeup_la_SOURCES = main.cpp
writeup_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
writeup_la_LIBADD = $(LIB_KOFFICECORE)

kdelnk_DATA = writeup.desktop
kdelnkdir = $(kde_appsdir)/Office

rcdir = $(kde_datadir)/writeup
rc_DATA = writeup.rc writeup_readonly.rc

kde_services_DATA = writeuppart.desktop

subdirs = pics
