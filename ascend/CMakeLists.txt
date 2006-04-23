kde4_header()
project(ascend)
include_directories(${KOFFICECORE_INCLUDES} ${CMAKE_SOURCE_DIR}/kword/ ${KOTEXT_INCLUDES}) 


########### next target ###############

set(writeup_part_PART_SRCS WUDocument.cpp WUFactory.cpp WUView.cpp )

kde4_automoc(${writeup_part_PART_SRCS})

kde4_add_plugin(writeup_part WITH_PREFIX ${writeup_part_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} writeup_part )

target_link_libraries(writeup_part  ${KDE4_KDECORE_LIBS} kwordprivate )

install_targets(${PLUGIN_INSTALL_DIR} writeup_part )


########### next target ###############

set(writeup_KDEINIT_SRCS main.cpp )

kde4_automoc(${writeup_KDEINIT_SRCS})

kde4_add_kdeinit_executable( writeup ${writeup_KDEINIT_SRCS})

target_link_libraries(kdeinit_writeup  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${LIB_INSTALL_DIR} kdeinit_writeup )

target_link_libraries( writeup kdeinit_writeup )
install_targets(/bin writeup )

########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  writeuppart.desktop )
install_files( ${DATA_INSTALL_DIR}/writeup FILES  writeup.rc writeup_readonly.rc )
install_files( ${APPLNK_INSTALL_DIR}/Office FILES  writeup.desktop )

kde4_footer()



#original Makefile.am contents follow:

#AM_CPPFLAGS = -I$(top_srcdir)/kword $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES = libwriteup_part.la
#libwriteup_part_la_LIBADD = ../kword/libkwordprivate.la
#libwriteup_part_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#libwriteup_part_la_SOURCES = WUDocument.cpp WUFactory.cpp WUView.cpp
#
#METASOURCES = AUTO
#
### The kdeinit loadable module and the writeup binary
#kdeinit_LTLIBRARIES = writeup.la
#lib_LTLIBRARIES =
#bin_PROGRAMS =
#writeup_la_SOURCES = main.cpp
#writeup_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#writeup_la_LIBADD = $(LIB_KOFFICECORE)
#
#kdelnk_DATA = writeup.desktop
#kdelnkdir = $(kde_appsdir)/Office
#
#rcdir = $(kde_datadir)/writeup
#rc_DATA = writeup.rc writeup_readonly.rc
#
#kde_services_DATA = writeuppart.desktop
#
#subdirs = pics
