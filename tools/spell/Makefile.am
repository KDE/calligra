
INCLUDES = -I$(srcdir)/../../lib/kofficecore -I$(idldir) $(all_includes)
libkspelltool_la_LIBADD = ../../lib/kofficecore/libkofficecore.la -lkspell

####### Files

kde_module_LTLIBRARIES = libkspelltool.la

libkspelltool_la_SOURCES = main.cc

libkspelltool_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined

noinst_HEADERS = main.h

libkspelltool_la_METASOURCES = main.moc

service_DATA = kspelltool.desktop
servicedir = $(kde_servicesdir)

