include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_report.la kexireportwidgets.la

kexihandler_report_la_SOURCES = kexireports.cpp
kexihandler_report_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module -no-undefined
kexihandler_report_la_LIBADD = ../../core/libkexicore.la \
  $(top_builddir)/kexi/widget/utils/libkexiguiutils.la \
  $(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	$(top_builddir)/kexi/formeditor/libkformdesigner.la \
  $(top_builddir)/kexi/plugins/forms/libkexiformutils.la \
  ./libkexireportutils.la

kexireportwidgets_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module -no-undefined
kexireportwidgets_la_SOURCES =   reportwidgets.cpp kexireportfactory.cpp
kexireportwidgets_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la \
	$(top_builddir)/kexi/plugins/forms/libkexiformutils.la \
	$(top_builddir)/kexi/plugins/forms/widgets/libkexiformutilswidgets.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	./libkexireportutils.la

lib_LTLIBRARIES = libkexireportutils.la
libkexireportutils_la_SOURCES = \
	kexireportpart.cpp kexireportview.cpp kexireportform.cpp
libkexireportutils_la_LDFLAGS = $(all_libraries) $(VER_INFO) -no-undefined
libkexireportutils_la_LIBADD = $(top_builddir)/kexi/core/libkexicore.la \
	$(top_builddir)/kexi/formeditor/libkformdesigner.la \
	$(top_builddir)/kexi/plugins/forms/widgets/libkexiformutilswidgets.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	$(top_builddir)/kexi/plugins/forms/libkexiformutils.la

kformdesignerservicesdir=$(kde_servicesdir)/kformdesigner
kformdesignerservices_DATA = kformdesigner_kexireportfactory.desktop

servicesdir=$(kde_servicesdir)/kexi
services_DATA=kexireporthandler.desktop

rcdir = $(kde_datadir)/kexi
rc_DATA = kexireportpartui.rc kexireportpartinstui.rc

SUBDIRS = .

INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/lib -I$(top_srcdir)/lib/kofficecore \
		-I$(top_srcdir)/kexi/widget/utils \
		-I$(top_srcdir)/kexi/widget \
		-I$(top_srcdir)/kexi/formeditor -I$(top_srcdir)/kexi/plugins/forms $(all_includes)

METASOURCES = AUTO

include ../Makefile.common
