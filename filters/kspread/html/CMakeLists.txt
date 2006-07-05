

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDES}  )


########### next target ###############

set(kspreadhtmlexport_PART_SRCS htmlexport.cc exportdialog.cc )

kde4_automoc(${kspreadhtmlexport_PART_SRCS})

kde4_add_ui3_files(kspreadhtmlexport_PART_SRCS exportwidget.ui )

kde4_add_plugin(kspreadhtmlexport WITH_PREFIX ${kspreadhtmlexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kspreadhtmlexport )

target_link_libraries(kspreadhtmlexport  ${KDE4_KDECORE_LIBS} kspreadcommon )

install(TARGETS kspreadhtmlexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_html_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
#	$(KOTEXT_INCLUDES) $(all_includes)
#
######## Files
#
#kde_module_LTLIBRARIES = libkspreadhtmlexport.la
#
#libkspreadhtmlexport_la_SOURCES = htmlexport.cc exportdialog.cc exportwidget.ui
#libkspreadhtmlexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#libkspreadhtmlexport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la
#
#METASOURCES = AUTO
#
#service_DATA = kspread_html_export.desktop
#
#servicedir = $(kde_servicesdir)
