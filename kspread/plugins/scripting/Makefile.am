#SUBDIRS = kspreadcore samples
SUBDIRS = kspreadcore scripts

INCLUDES = -I$(top_builddir)/lib/kross/main \
           -I$(top_srcdir)/kspread \
           $(KROSS_INCLUDES) \
           $(KOFFICE_INCLUDES) \
           $(all_includes)

kspreadscripting_la_SOURCES = scripting.cc

kde_module_LTLIBRARIES = kspreadscripting.la
noinst_HEADERS = scripting.h

kspreadscripting_la_METASOURCES = AUTO

kspreadscripting_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kspreadscripting_la_LIBADD = $(top_builddir)/kspread/libkspreadcommon.la $(top_builddir)/lib/kross/main/libkrossmain.la

kde_services_DATA = kspreadscripting.desktop

kspreadrcdir = $(kde_datadir)/kspread/kpartplugins
kspreadrc_DATA = scripting.rc
