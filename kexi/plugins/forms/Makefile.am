include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_form.la #kexidbwidgets.la

kexihandler_form_la_SOURCES = kexiformpart.cpp kexidbform.cpp
kexihandler_form_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module -no-undefined
kexihandler_form_la_LIBADD = ../../core/libkexicore.la ../../tableview/libkexidatatable.la ../../widget/libkexiextendedwidgets.la ../../formeditor/libkformeditor.la

#kexidbwidgets_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) -module
#kexidbwidgets_la_SOURCES =  kexidbfactory.cpp
#kexidbwidgets_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformeditor.la

servicesdir=$(kde_servicesdir)/kformeditor
services_DATA = kexidbfactory.desktop

SUBDIRS = .

INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/datatable \
		-I$(top_srcdir)/kexi/formeditor $(all_includes)

METASOURCES = AUTO

