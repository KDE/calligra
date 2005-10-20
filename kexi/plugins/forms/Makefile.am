include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_form.la kexidbwidgets.la

kexihandler_form_la_SOURCES = kexiforms.cpp

kexihandler_form_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module -no-undefined
kexihandler_form_la_LIBADD = $(top_builddir)/kexi/core/libkexicore.la \
	$(top_builddir)/kexi/widget/utils/libkexiguiutils.la \
	$(top_builddir)/kexi/widget/tableview/libkexidatatable.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	$(top_builddir)/kexi/formeditor/libkformdesigner.la \
	$(top_builddir)/lib/koproperty/libkoproperty.la \
	./libkexiformutils.la

kexidbwidgets_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
kexidbwidgets_la_SOURCES = kexidbfactory.cpp
kexidbwidgets_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la \
	./libkexiformutils.la

lib_LTLIBRARIES = libkexiformutils.la
libkexiformutils_la_SOURCES = kexiformdataiteminterface.cpp kexidataawarewidgetinfo.cpp \
	kexidataprovider.cpp kexiformscrollview.cpp kexidbform.cpp kexiformeventhandler.cpp \
	kexiactionselectiondialog.cpp kexiformmanager.cpp \
	kexidatasourcepage.cpp kexiformpart.cpp kexiformview.cpp \
	kexidbfieldedit.cpp kexilabel.cpp kexidbwidgets.cpp 
libkexiformutils_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved
libkexiformutils_la_LIBADD = $(top_builddir)/kexi/core/libkexicore.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	$(top_builddir)/kexi/plugins/forms/widgets/libkexiformutilswidgets.la
servicesdir=$(kde_servicesdir)/kformdesigner
services_DATA = kexidbfactory.desktop

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
noinst_HEADERS = kexidbinputwidget.h kexidataprovider.h kexidbfactory.h \
	kexidbform.h kexiformpart.h kexiformscrollview.h kexiformview.h kexilabel.h \
	kexiwidgetfactory.h
