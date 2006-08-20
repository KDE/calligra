####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread -I$(top_srcdir)/kexi $(KOFFICE_INCLUDES) \
	$(KOTEXT_INCLUDES)  $(all_includes)

####### Files

kde_module_LTLIBRARIES = libkspreadkexiimport.la

libkspreadkexiimport_la_SOURCES = kspread_kexiimport.cc \
				kspread_kexiimportdialogbase.ui \
				kspread_kexiimportdialog.cc
libkspreadkexiimport_la_LDFLAGS = -avoid-version -module -no-undefined \
	$(all_libraries)
libkspreadkexiimport_la_LIBADD = ../../../kexi/kexidb/libkexidb.la \
				../../../kexi/kexidb/parser/libkexidbparser.la \
				../../../kspread/libkspreadcommon.la

METASOURCES = AUTO

service_DATA = kspread_kexi_import.desktop
servicedir = $(kde_servicesdir)
