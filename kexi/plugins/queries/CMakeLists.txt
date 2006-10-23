

include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib/kofficecore ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )


########### next target ###############

set(kexihandler_query_PART_SRCS 
   kexiquerypart.cpp 
   kexiquerydesignersql.cpp 
   kexiquerydesignersqlhistory.cpp 
   kexiquerydesignerguieditor.cpp 
   kexiqueryview.cpp )

kde4_automoc(${kexihandler_query_PART_SRCS})

kde4_add_plugin(kexihandler_query ${kexihandler_query_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kexihandler_query )

target_link_libraries(kexihandler_query  ${KDE4_KDECORE_LIBS} kexicore kexidb kexiextendedwidgets kexidatatable kexirelationsview koproperty )

install(TARGETS kexihandler_query  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexiquerypartui.rc kexiquerypartinstui.rc  DESTINATION ${DATA_INSTALL_DIR}/kexi)
install( FILES  kexiqueryhandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexihandler_query.la
#
#kexihandler_query_la_SOURCES = kexiquerypart.cpp kexiquerydesignersql.cpp \
#				kexiquerydesignersqlhistory.cpp kexiquerydesignerguieditor.cpp \
#				kexiqueryview.cpp
#kexihandler_query_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module -no-undefined
#kexihandler_query_la_LIBADD = ../../core/libkexicore.la \
#  $(top_builddir)/kexi/kexidb/libkexidb.la \
#  $(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
#  $(top_builddir)/kexi/widget/tableview/libkexidatatable.la \
#  $(top_builddir)/kexi/widget/relations/libkexirelationsview.la \
#  $(top_builddir)/lib/koproperty/libkoproperty.la
#
#INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
#	-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/widget/tableview \
#	-I$(top_srcdir)/lib -I$(top_srcdir)/lib/kofficecore \
#	-I$(top_srcdir)/kexi/kexidb $(all_includes)
#
#servicesdir=$(kde_servicesdir)/kexi
#services_DATA=kexiqueryhandler.desktop
#
#rcdir = $(kde_datadir)/kexi
#rc_DATA = kexiquerypartui.rc kexiquerypartinstui.rc
#
#METASOURCES = AUTO
#
#include ../Makefile.common
