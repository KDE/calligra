

add_subdirectory( kabc )
add_subdirectory( kspread )

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(SQLDIRECTORY)")
include_directories( ${CMAKE_SOURCE_DIR}/lib/kformula ${CMAKE_SOURCE_DIR}/kword ${KOTEXT_INCLUDES} ${KOFFICEUI_INCLUDES} )


########### next target ###############

set(kwmailmerge_classic_PART_SRCS KWClassicSerialDataSource.cpp )

kde4_automoc(${kwmailmerge_classic_PART_SRCS})

kde4_add_plugin(kwmailmerge_classic ${kwmailmerge_classic_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwmailmerge_classic )

target_link_libraries(kwmailmerge_classic  ${KDE4_KDECORE_LIBS} kwmailmerge_interface ${KDE4_KDEUI_LIBS} kotext )

install_targets(${PLUGIN_INSTALL_DIR} kwmailmerge_classic )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kwserialletter_classic.desktop )





#original Makefile.am contents follow:

#
#KDE_CXXFLAGS = $(USE_RTTI)
#INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula $(KOTEXT_INCLUDES) -I$(top_srcdir)/kword $(all_includes)
#
#kde_module_LTLIBRARIES = kwmailmerge_classic.la
#
### Plugin encapsulating the (old) internally stored style
#kwmailmerge_classic_la_SOURCES = KWClassicSerialDataSource.cpp
#kwmailmerge_classic_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kwmailmerge_classic_la_LIBADD = ../libkwmailmerge_interface.la $(LIB_KDEUI) $(LIB_KOTEXT)
#
#METASOURCES = AUTO
#
#services_DATA=kwserialletter_classic.desktop
#servicesdir=$(kde_servicesdir)
#
#if include_sql
#SQLDIRECTORY=sql
#endif
#
#SUBDIRS = . kabc kspread $(SQLDIRECTORY)
#
