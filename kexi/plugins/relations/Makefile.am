include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_relation.la

#kexihandler_relation_la_SOURCES = kexirelationhandler.cpp kexirelationhandlerproxy.cpp kexirelationview.cpp \
#				kexirelationviewtable.cpp kexirelationdialog.cpp \
#				kexirelationviewconnection.cpp
kexihandler_relation_la_SOURCES = kexirelationpartimpl.cpp kexirelationview.cpp kexirelationviewconnection.cpp \
				kexirelationviewtable.cpp kexirelationmaindlg.cpp \
				kexirelationdialog.cpp
kexihandler_relation_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexihandler_relation_la_LIBADD = ../../core/libkexicore.la

INCLUDES= $(KOFFICE_INCLUDES) -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/tableview \
		-I$(top_srcdir)/kexi/kexiDB $(all_includes)

METASOURCES = AUTO

include ../Makefile.common
