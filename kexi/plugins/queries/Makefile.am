include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_query.la

#kexihandler_query_la_SOURCES = kexiquerypart.cpp kexiquerydesigner.cpp kexiquerypartitem.cpp kexiquerypartproxy.cpp \
#				kexiquerydesignerguieditor.cpp kexiquerydesignersql.cpp  \
#				kexiquerydesignersqlhistory.cpp kexiparameterlisteditor.ui kexiaddparamwidget.ui \
#				kexiaddparamdialog.cpp kexidynamicqueryparameterdialog.cpp

kexihandler_query_la_SOURCES = kexiquerypart.cpp kexiquerydesignersql.cpp \
				kexiquerydesignersqlhistory.cpp kexiquerydesignerguieditor.cpp \
				kexiqueryview.cpp
kexihandler_query_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module -no-undefined
kexihandler_query_la_LIBADD = ../../core/libkexicore.la \
	../../kexidb/parser/libkexidbparser.la ../../tableview/libkexidatatable.la \
	../../widget/libkexiextendedwidgets.la ../../widget/relations/libkexirelationsview.la

INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
	-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/tableview \
	-I$(top_srcdir)/kexi/kexiDB $(all_includes)

METASOURCES = AUTO

include ../Makefile.common
