include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = libkexihandler_csv_importexport.la

libkexihandler_csv_import_la_SOURCES = kexicsv_importexportpart.cpp kexicsvimportdialog.cpp \
	kexicsvimportoptionsdlg.cpp kexicsvwidgets.cpp

libkexihandler_csv_import_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
libkexihandler_csv_import_la_LIBADD = ../../../core/libkexicore.la \
	../../../migration/libkeximigrate.la

INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/migration \
		-I$(top_srcdir)/kexi/kexiDB $(all_includes)

METASOURCES = AUTO

servicesdir=$(kde_servicesdir)/kexi
services_DATA=kexicsv_importhandler.desktop

include ../../Makefile.common
