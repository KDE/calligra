
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula \
	$(KOTEXT_INCLUDES) $(all_includes) -I$(top_srcdir)/kword

kde_module_LTLIBRARIES = libkwserialletter_classic.la libkwserialletter_qtsqldb.la

## Plugin encapsulating the (old) internally stored style
libkwserialletter_classic_la_SOURCES = serialletter_classicplugin.cc
libkwserialletter_classic_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkwserialletter_classic_la_LIBADD = ../libkwserialletter_interface.la $(LIB_KDEUI)

## Plugin encapsulating the QT SQL database interface
libkwserialletter_qtsqldb_la_SOURCES = serialletter_qtsqlplugin.cc
libkwserialletter_qtsqldb_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkwserialletter_qtsqldb_la_LIBADD = ../libkwserialletter_interface.la $(LIB_KDEUI)

METASOURCES = AUTO

services_DATA=kwserialletter_classic.desktop kwserialletter_qtsqldb.desktop
servicesdir=$(kde_servicesdir)

SUBDIRS = .
