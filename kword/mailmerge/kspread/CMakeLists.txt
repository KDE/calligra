

include_directories( 
	${CMAKE_SOURCE_DIR}/kspread 
	${CMAKE_SOURCE_DIR}/kword 
	${KOTEXT_INCLUDES} 
	${KOFFICEUI_INCLUDES} )


########### next target ###############

set(kwmailmerge_kspread_PART_SRCS kwmailmerge_kspread.cpp kwmailmerge_kspread_config.cpp )

kde4_automoc(${kwmailmerge_kspread_PART_SRCS})

kde4_add_plugin(kwmailmerge_kspread ${kwmailmerge_kspread_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwmailmerge_kspread )

target_link_libraries(kwmailmerge_kspread  ${KDE4_KDECORE_LIBS} kwmailmerge_interface kspreadcommon ${KDE4_KDEUI_LIBS} )

install(TARGETS kwmailmerge_kspread  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kwmailmerge_kspread.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#
#KDE_CXXFLAGS = $(USE_RTTI)
#INCLUDES = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES)  \
#           -I$(top_srcdir)/kspread -I$(top_srcdir)/kword $(all_includes)
#
#kde_module_LTLIBRARIES = kwmailmerge_kspread.la
#
#kwmailmerge_kspread_la_SOURCES = kwmailmerge_kspread.cpp kwmailmerge_kspread_config.cpp
#kwmailmerge_kspread_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kwmailmerge_kspread_la_LIBADD = ../../libkwmailmerge_interface.la \
#                                $(top_builddir)/kspread/libkspreadcommon.la $(LIB_KDEUI)
#
#METASOURCES = AUTO
#
#services_DATA = kwmailmerge_kspread.desktop
#servicesdir = $(kde_servicesdir)
