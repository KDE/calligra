kde4_header()




########### next target ###############

set(asciiimport_PART_SRCS asciiimport.cc ImportDialog.cc )

kde4_automoc(${asciiimport_PART_SRCS})

kde4_add_ui3_files(asciiimport_PART_SRCS ImportDialogUI.ui )

kde4_add_plugin(asciiimport WITH_PREFIX ${asciiimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} asciiimport )

target_link_libraries(asciiimport  ${KDE4_KDEUI_LIBS} kofficecore kwordexportfilters)

install_targets(${LIB_INSTALL_DIR}/kde4 asciiimport )


########### next target ###############

set(asciiexport_PART_SRCS asciiexport.cc ExportDialog.cc )

kde4_automoc(${asciiexport_PART_SRCS})

kde4_add_ui3_files(asciiexport_PART_SRCS ExportDialogUI.ui )

kde4_add_plugin(asciiexport WITH_PREFIX ${asciiexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} asciiexport )

target_link_libraries(asciiexport  ${KDE4_KDEUI_LIBS} kofficecore kwordexportfilters)

install_targets(${LIB_INSTALL_DIR}/kde4 asciiexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_ascii_import.desktop kword_ascii_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)
#
#libasciiimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libasciiimport_la_LIBADD = $(KOFFICE_LIBS)
#
#libasciiexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libasciiexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la
#
######## Files
#
#kde_module_LTLIBRARIES = libasciiimport.la libasciiexport.la
#
#libasciiimport_la_SOURCES = asciiimport.cc ImportDialogUI.ui ImportDialog.cc
#
#libasciiexport_la_SOURCES = asciiexport.cc ExportDialogUI.ui ExportDialog.cc
#
#noinst_HEADERS = asciiimport.h ImportDialog.h asciiexport.h ExportDialog.h
#
#METASOURCES = AUTO
##libasciiimport_la_METASOURCES = asciiimport.moc ImportDialog.moc
##libasciiexport_la_METASOURCES = asciiexport.moc ExportDialog.moc
#
#service_DATA = kword_ascii_import.desktop kword_ascii_export.desktop
#
#servicedir = $(kde_servicesdir)
#
