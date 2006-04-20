kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib/kofficecore ${CMAKE_SOURCE_DIR}/kexi/widget/utils ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi/plugins/forms ${KDE4_INCLUDES}  )


########### next target ###############

set(kexihandler_report_PART_SRCS kexireports.cpp )

kde4_automoc(${kexihandler_report_PART_SRCS})

kde4_add_plugin(kexihandler_report ${kexihandler_report_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kexihandler_report )

target_link_libraries(kexihandler_report  ${KDE4_KDECORE_LIBS} kexicore kexiguiutils kexiextendedwidgets kformdesigner kexiformutils kexireportutils )

install_targets(${PLUGIN_INSTALL_DIR} kexihandler_report )


########### next target ###############

set(kexireportwidgets_PART_SRCS reportwidgets.cpp kexireportfactory.cpp )

kde4_automoc(${kexireportwidgets_PART_SRCS})

kde4_add_plugin(kexireportwidgets ${kexireportwidgets_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kexireportwidgets )

target_link_libraries(kexireportwidgets  ${KDE4_KDECORE_LIBS} kformdesigner kexireportutils )

install_targets(${PLUGIN_INSTALL_DIR} kexireportwidgets )


########### next target ###############

set(kexireportutils_LIB_SRCS kexireportpart.cpp kexireportview.cpp kexireportform.cpp )

kde4_automoc(${kexireportutils_LIB_SRCS})

kde4_add_library(kexireportutils SHARED ${kexireportutils_LIB_SRCS})

target_link_libraries(kexireportutils  ${KDE4_KDECORE_LIBS} kexicore kexiextendedwidgets kexiformutilswidgets )

set_target_properties(kexireportutils PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} kexireportutils )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kexi FILES  kexireportpartui.rc kexireportpartinstui.rc )
install_files( ${SERVICES_INSTALL_DIR}/kformdesigner FILES  kformdesigner_kexireportfactory.desktop )
install_files( ${SERVICES_INSTALL_DIR}/kexi FILES  kexireporthandler.desktop )

kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexihandler_report.la kexireportwidgets.la
#
#kexihandler_report_la_SOURCES = kexireports.cpp
#kexihandler_report_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module -no-undefined
#kexihandler_report_la_LIBADD = ../../core/libkexicore.la \
#  ../../widget/utils/libkexiguiutils.la \
#  ../../widget/libkexiextendedwidgets.la \
#  ../../formeditor/libkformdesigner.la \
#  ../forms/libkexiformutils.la \
#  ./libkexireportutils.la
#
#kexireportwidgets_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
#kexireportwidgets_la_SOURCES =   reportwidgets.cpp kexireportfactory.cpp
#kexireportwidgets_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la \
#	./libkexireportutils.la
#
#lib_LTLIBRARIES = libkexireportutils.la
#libkexireportutils_la_SOURCES = \
#	kexireportpart.cpp kexireportview.cpp kexireportform.cpp
#libkexireportutils_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved
#libkexireportutils_la_LIBADD = $(top_builddir)/kexi/core/libkexicore.la \
#	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
#	$(top_builddir)/kexi/plugins/forms/widgets/libkexiformutilswidgets.la
#
#kformdesignerservicesdir=$(kde_servicesdir)/kformdesigner
#kformdesignerservices_DATA = kformdesigner_kexireportfactory.desktop
#
#servicesdir=$(kde_servicesdir)/kexi
#services_DATA=kexireporthandler.desktop
#
#rcdir = $(kde_datadir)/kexi
#rc_DATA = kexireportpartui.rc kexireportpartinstui.rc
#
#SUBDIRS = .
#
#INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
#		-I$(top_srcdir)/lib -I$(top_srcdir)/lib/kofficecore \
#		-I$(top_srcdir)/kexi/widget/utils \
#		-I$(top_srcdir)/kexi/widget \
#		-I$(top_srcdir)/kexi/formeditor -I$(top_srcdir)/kexi/plugins/forms $(all_includes)
#
#METASOURCES = AUTO
#
#include ../Makefile.common
