include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_query.la

kexihandler_query_la_SOURCES = kexiquerypart.cpp kexiquerydesignersql.cpp \
				kexiquerydesignersqlhistory.cpp kexiquerydesignerguieditor.cpp \
				kexiqueryview.cpp
kexihandler_query_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module -no-undefined
kexihandler_query_la_LIBADD = ../../core/libkexicore.la \
  $(top_builddir)/kexi/kexidb/libkexidb.la \
  $(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
  $(top_builddir)/kexi/widget/tableview/libkexidatatable.la \
  $(top_builddir)/kexi/widget/relations/libkexirelationsview.la \
  $(top_builddir)/lib/koproperty/libkoproperty.la

INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
	-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/widget/tableview \
	-I$(top_srcdir)/lib -I$(top_srcdir)/lib/kofficecore \
	-I$(top_srcdir)/kexi/kexidb $(all_includes)

servicesdir=$(kde_servicesdir)/kexi
services_DATA=kexiqueryhandler.desktop

rcdir = $(kde_datadir)/kexi
rc_DATA = kexiquerypartui.rc kexiquerypartinstui.rc

METASOURCES = AUTO

include ../Makefile.common
