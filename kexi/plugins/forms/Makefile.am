kde_module_LTLIBRARIES = kexihandler_form.la

kexihandler_form_la_SOURCES = kexiformhandler.cpp kexiformhandleritem.cpp kexiformhandlerproxy.cpp kexidbwidgetcontainer.cpp kexidbwidgetfactory.cpp
kexihandler_form_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexihandler_form_la_LIBADD = ../../core/libkexicore.la  ../../formeditor/libkformeditor.la

SUBDIRS = .

INCLUDES= $(KOFFICE_INCLUDES) -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/datatable $(all_includes)

METASOURCES = AUTO

include $(top_srcdir)/kexi/Makefile.global
