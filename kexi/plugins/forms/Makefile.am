include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_form.la kformdesigner_kexidbwidgets.la

kexihandler_form_la_SOURCES = kexiforms.cpp

kexihandler_form_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module -no-undefined
kexihandler_form_la_LIBADD = $(top_builddir)/kexi/core/libkexicore.la \
	$(top_builddir)/kexi/widget/utils/libkexiguiutils.la \
	$(top_builddir)/kexi/widget/tableview/libkexidatatable.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	$(top_builddir)/kexi/formeditor/libkformdesigner.la \
	$(top_builddir)/lib/koproperty/libkoproperty.la \
	./libkexiformutils.la

kformdesigner_kexidbwidgets_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module -no-undefined
kformdesigner_kexidbwidgets_la_SOURCES = kexidbfactory.cpp
kformdesigner_kexidbwidgets_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	./libkexiformutils.la

lib_LTLIBRARIES = libkexiformutils.la
libkexiformutils_la_SOURCES = kexiformdataiteminterface.cpp kexidataawarewidgetinfo.cpp \
	kexidataprovider.cpp kexiformscrollview.cpp kexiformeventhandler.cpp \
	kexidbtextwidgetinterface.cpp kexiactionselectiondialog.cpp kexiformmanager.cpp \
	kexidatasourcepage.cpp kexiformpart.cpp kexiformview.cpp
libkexiformutils_la_LDFLAGS = $(all_libraries) $(VER_INFO) -no-undefined
libkexiformutils_la_LIBADD = $(top_builddir)/kexi/core/libkexicore.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	$(top_builddir)/kexi/formeditor/libkformdesigner.la \
	$(top_builddir)/kexi/plugins/forms/widgets/libkexiformutilswidgets.la

kformdesignerservicesdir=$(kde_servicesdir)/kformdesigner
kformdesignerservices_DATA=kformdesigner_kexidbfactory.desktop

servicesdir=$(kde_servicesdir)/kexi
services_DATA=kexiformhandler.desktop

rcdir = $(kde_datadir)/kexi
rc_DATA = kexiformpartui.rc kexiformpartinstui.rc

SUBDIRS = widgets .

INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget/utils \
		-I$(top_srcdir)/kexi/widget \
		-I$(top_srcdir)/kexi/formeditor \
		-I$(top_srcdir)/lib -I$(top_srcdir)/lib/koproperty -I$(top_srcdir)/lib/kofficecore \
		-I$(top_srcdir)/kexi/widget/tableview/private \
		-I$(top_srcdir)/kexi/widget/tableview $(all_includes)

METASOURCES = AUTO

include ../Makefile.common
noinst_HEADERS = kexidataprovider.h kexidbfactory.h \
	kexiformpart.h kexiformscrollview.h kexiformview.h