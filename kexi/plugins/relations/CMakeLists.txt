

include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/widget/relations ${CMAKE_SOURCE_DIR}/kexi/tableview ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )


########### next target ###############

set(kexihandler_relation_PART_SRCS kexirelationpartimpl.cpp kexirelationmaindlg.cpp )

kde4_automoc(${kexihandler_relation_PART_SRCS})

kde4_add_plugin(kexihandler_relation ${kexihandler_relation_PART_SRCS})



target_link_libraries(kexihandler_relation  ${KDE4_KDECORE_LIBS} kexicore kexirelationsview )

install(TARGETS kexihandler_relation  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexirelationpartui.rc kexirelationpartinstui.rc  DESTINATION ${DATA_INSTALL_DIR}/kexi)
install( FILES  kexirelationhandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexihandler_relation.la
#
##kexihandler_relation_la_SOURCES = kexirelationhandler.cpp kexirelationhandlerproxy.cpp kexirelationview.cpp \
##				kexirelationviewtable.cpp kexirelationdialog.cpp \
##				kexirelationviewconnection.cpp
#kexihandler_relation_la_SOURCES = kexirelationpartimpl.cpp \
#				kexirelationmaindlg.cpp
#
#kexihandler_relation_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
#kexihandler_relation_la_LIBADD = ../../core/libkexicore.la \
#	../../widget/relations/libkexirelationsview.la
#
#INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
#		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/widget/relations \
#		-I$(top_srcdir)/kexi/tableview \
#		-I$(top_srcdir)/kexi/kexidb $(all_includes)
#
#servicesdir=$(kde_servicesdir)/kexi
#services_DATA=kexirelationhandler.desktop
#
#rcdir = $(kde_datadir)/kexi
#rc_DATA = kexirelationpartui.rc kexirelationpartinstui.rc
#
#METASOURCES = AUTO
#
#include ../Makefile.common
