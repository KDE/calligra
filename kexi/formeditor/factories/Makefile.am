include $(top_srcdir)/kexi/Makefile.global

INCLUDES = -I$(top_srcdir)/kexi/formeditor $(all_includes)
kde_module_LTLIBRARIES = containers.la
containers_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) -module
containers_la_SOURCES =  containerfactory.cpp
containers_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformeditor.la

servicesdir=$(kde_servicesdir)/kformeditor
services_DATA = containerfactory.desktop stdwidgets.desktop

METASOURCES = AUTO

#libstdwidgets_la_LDFLAGS = 
#libstdwidgets_la_SOURCES =  stdwidgets.h stdwidgets.cpp
#libstdwidgets_la_LIBADD = $(top_builddir)kexi/formeditor/libkformeditor.la

