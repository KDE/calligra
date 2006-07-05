

include_directories( ${CMAKE_SOURCE_DIR}/lib/kformula ${CMAKE_SOURCE_DIR}/kword ${KOTEXT_INCLUDES} ${KOFFICEUI_INCLUDES} )


########### next target ###############

set(kwmailmerge_kabc_PART_SRCS KWMailMergeKABC.cpp KWMailMergeKABCConfig.cpp )

kde4_automoc(${kwmailmerge_kabc_PART_SRCS})

kde4_add_ui3_files(kwmailmerge_kabc_PART_SRCS addresspicker.ui )

kde4_add_plugin(kwmailmerge_kabc ${kwmailmerge_kabc_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwmailmerge_kabc )

target_link_libraries(kwmailmerge_kabc ${KDE4_KDEUI_LIBS} kwmailmerge_interface kabc )

install(TARGETS kwmailmerge_kabc  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kwmailmerge_kabc.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#
#KDE_CXXFLAGS = $(USE_RTTI)
#INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula \
#	$(KOTEXT_INCLUDES) -I$(top_srcdir)/kword $(all_includes)
#
#kde_module_LTLIBRARIES = kwmailmerge_kabc.la
#
### Plugin encapsulating the QT SQL database interface
#kwmailmerge_kabc_la_SOURCES = KWMailMergeKABC.cpp KWMailMergeKABCConfig.cpp addresspicker.ui
#kwmailmerge_kabc_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kwmailmerge_kabc_la_LIBADD = ../../libkwmailmerge_interface.la $(LIB_KDEUI) $(LIB_KABC)
#
#METASOURCES = AUTO
#
#services_DATA=kwmailmerge_kabc.desktop
#servicesdir=$(kde_servicesdir)
#
