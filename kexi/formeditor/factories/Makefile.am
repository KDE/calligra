include $(top_srcdir)/kexi/Makefile.global

INCLUDES = -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/formeditor \
	-I$(top_srcdir)/kexi/core $(all_includes) \
	-I$(top_srcdir)/lib -I$(top_srcdir)/lib/kofficecore
kde_module_LTLIBRARIES = containers.la stdwidgets.la
containers_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
containers_la_SOURCES =  containerfactory.cpp
containers_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la

stdwidgets_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
stdwidgets_la_SOURCES =  stdwidgetfactory.cpp
stdwidgets_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la


servicesdir=$(kde_servicesdir)/kformdesigner
services_DATA = containerfactory.desktop stdwidgets.desktop

METASOURCES = AUTO

