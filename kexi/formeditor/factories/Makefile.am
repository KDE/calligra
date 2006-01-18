include $(top_srcdir)/kexi/Makefile.global

INCLUDES = -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/formeditor \
	-I$(top_srcdir)/kexi/core \
	-I$(top_srcdir)/lib -I$(top_srcdir)/lib/koproperty -I$(top_srcdir)/lib/kofficecore $(all_includes)
kde_module_LTLIBRARIES = kformdesigner_containers.la kformdesigner_stdwidgets.la
kformdesigner_containers_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
kformdesigner_containers_la_SOURCES =  containerfactory.cpp
kformdesigner_containers_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la

kformdesigner_stdwidgets_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
kformdesigner_stdwidgets_la_SOURCES =  stdwidgetfactory.cpp
kformdesigner_stdwidgets_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la


servicesdir=$(kde_servicesdir)/kformdesigner
services_DATA = kformdesigner_containers.desktop kformdesigner_stdwidgets.desktop

METASOURCES = AUTO

