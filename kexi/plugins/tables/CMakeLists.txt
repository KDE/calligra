

include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget
 ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/kexi/kexidb ${CMAKE_SOURCE_DIR}/lib ${KDE4_INCLUDES}  )

########### next target ###############

set(kexihandler_table_PART_SRCS kexitablepart.cpp kexitabledesignerview.cpp
  kexitabledesigner_dataview.cpp kexitabledesignercommands.cpp
  kexitabledesignerview_p.cpp kexilookupcolumnpage.cpp )


kde4_add_plugin(kexihandler_table ${kexihandler_table_PART_SRCS})



target_link_libraries(kexihandler_table  ${KDE4_KDECORE_LIBS} kexicore kexidb kexidatatable kexiextendedwidgets koproperty )

install(TARGETS kexihandler_table  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexitablepartui.rc kexitablepartinstui.rc  DESTINATION ${DATA_INSTALL_DIR}/kexi)
install( FILES  kexitablehandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexihandler_table.la
#
#kexihandler_table_la_SOURCES = kexitablepart.cpp kexialtertabledialog.cpp kexialtertable_dataview.cpp
#kexihandler_table_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
#kexihandler_table_la_LIBADD = $(top_builddir)/kexi/core/libkexicore.la \
#	$(top_builddir)/kexi/kexidb/libkexidb.la \
#	$(top_builddir)/kexi/widget/tableview/libkexidatatable.la \
#	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
#	$(top_builddir)/lib/koproperty/libkoproperty.la
#
#INCLUDES= $(KOFFICE_INCLUDES) \
#	-I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
#	-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/widget/tableview \
#	-I$(top_srcdir)/kexi/kexidb -I$(top_srcdir)/lib  $(all_includes)
#
#servicesdir=$(kde_servicesdir)/kexi
#services_DATA=kexitablehandler.desktop
#
#rcdir = $(kde_datadir)/kexi
#rc_DATA = kexitablepartui.rc kexitablepartinstui.rc
#
#METASOURCES = AUTO
#
#include ../Makefile.common
