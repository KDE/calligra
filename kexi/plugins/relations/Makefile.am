include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_relation.la

#kexihandler_relation_la_SOURCES = kexirelationhandler.cpp kexirelationhandlerproxy.cpp kexirelationview.cpp \
#				kexirelationviewtable.cpp kexirelationdialog.cpp \
#				kexirelationviewconnection.cpp
kexihandler_relation_la_SOURCES = kexirelationpartimpl.cpp \
				kexirelationmaindlg.cpp

kexihandler_relation_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
kexihandler_relation_la_LIBADD = ../../core/libkexicore.la \
	../../widget/relations/libkexirelationsview.la

INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/widget/relations \
		-I$(top_srcdir)/kexi/tableview \
		-I$(top_srcdir)/kexi/kexidb $(all_includes)

servicesdir=$(kde_servicesdir)/kexi
services_DATA=kexirelationhandler.desktop

rcdir = $(kde_datadir)/kexi
rc_DATA = kexirelationpartui.rc kexirelationpartinstui.rc

METASOURCES = AUTO

include ../Makefile.common
