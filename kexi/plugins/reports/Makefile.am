include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_report.la kexireportwidgets.la

kexihandler_report_la_SOURCES = kexireports.cpp
kexihandler_report_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module -no-undefined
kexihandler_report_la_LIBADD = ../../core/libkexicore.la \
  ../../widget/utils/libkexiguiutils.la \
  ../../widget/libkexiextendedwidgets.la \
  ../../formeditor/libkformdesigner.la \
  ../forms/libkexiformutils.la \
  ./libkexireportutils.la

kexireportwidgets_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
kexireportwidgets_la_SOURCES =   reportwidgets.cpp kexireportfactory.cpp
kexireportwidgets_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la \
	./libkexireportutils.la

lib_LTLIBRARIES = libkexireportutils.la
libkexireportutils_la_SOURCES = \
	kexireportpart.cpp kexireportview.cpp kexireportform.cpp
libkexireportutils_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved
libkexireportutils_la_LIBADD = $(top_builddir)/kexi/core/libkexicore.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	$(top_builddir)/kexi/plugins/forms/widgets/libkexiformutilswidgets.la

servicesdir=$(kde_servicesdir)/kformdesigner
services_DATA = kexireportfactory.desktop

SUBDIRS = .

INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/lib -I$(top_srcdir)/lib/kofficecore \
		-I$(top_srcdir)/kexi/widget/utils \
		-I$(top_srcdir)/kexi/widget \
		-I$(top_srcdir)/kexi/formeditor -I$(top_srcdir)/kexi/plugins/forms $(all_includes)

METASOURCES = AUTO

include ../Makefile.common
