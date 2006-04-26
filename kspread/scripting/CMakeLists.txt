kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )
add_subdirectory( kspreadcore )

########### next target ###############

set(kspreadscripting_PART_SRCS scripting.cc )

kde4_automoc(${kspreadscripting_PART_SRCS})

kde4_add_plugin(kspreadscripting ${kspreadscripting_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kspreadscripting )

target_link_libraries(kspreadscripting  ${KDE4_KDECORE_LIBS} kspreadcommon krossmain )

install_targets(${PLUGIN_INSTALL_DIR} kspreadscripting )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kspreadscripting.desktop )
install_files( ${DATA_INSTALL_DIR}/kspread/kpartplugins FILES  scripting.rc )

kde4_footer()



#original Makefile.am contents follow:

##SUBDIRS = kspreadcore samples
#SUBDIRS = kspreadcore
#
#INCLUDES = -I$(top_builddir)/lib/kross/main \
#           -I$(top_srcdir)/kspread \
#           $(KROSS_INCLUDES) \
#           $(KOFFICE_INCLUDES) \
#           $(all_includes)
#
#kspreadscripting_la_SOURCES = scripting.cc
#
#kde_module_LTLIBRARIES = kspreadscripting.la
#noinst_HEADERS = scripting.h
#
#kspreadscripting_la_METASOURCES = AUTO
#
#kspreadscripting_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kspreadscripting_la_LIBADD = $(top_builddir)/kspread/libkspreadcommon.la $(top_builddir)/lib/kross/main/libkrossmain.la
#
#kde_services_DATA = kspreadscripting.desktop
#
#kspreadrcdir = $(kde_datadir)/kspread/kpartplugins
#kspreadrc_DATA = scripting.rc
