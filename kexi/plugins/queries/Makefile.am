include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_query.la

kexihandler_query_la_SOURCES = kexiquerypart.cpp kexiquerydesignersql.cpp \
				kexiquerydesignersqlhistory.cpp kexiquerydesignerguieditor.cpp \
				kexiqueryview.cpp
kexihandler_query_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module -no-undefined
kexihandler_query_la_LIBADD = ../../core/libkexicore.la \
  ../../kexidb/libkexidb.la \
  ../../widget/libkexiextendedwidgets.la \
  ../../widget/tableview/libkexidatatable.la \
  ../../widget/relations/libkexirelationsview.la

INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
	-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/widget/tableview \
	-I$(top_srcdir)/kexi/kexidb $(all_includes)

METASOURCES = AUTO

include ../Makefile.common
