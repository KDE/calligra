kde4_header()




########### next target ###############

set(htmlexport_PART_SRCS 
   htmlexport.cc 
   ExportDialog.cc 
   ExportFilter.cc 
   ExportCss.cc 
   ExportDocStruct.cc 
   ExportBasic.cc )

kde4_automoc(${htmlexport_PART_SRCS})

kde4_add_ui3_files(htmlexport_PART_SRCS ExportDialogUI.ui )

kde4_add_plugin(htmlexport WITH_PREFIX ${htmlexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} htmlexport kwordexportfilters ${KDE4_KDEUI_LIBS} )

target_link_libraries(htmlexport  ${KDE4_KDECORE_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} htmlexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_html_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../../libexport $(all_includes)
#
#libhtmlexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#
#libhtmlexport_la_LIBADD = $(KOFFICE_LIBS) ../../libexport/libkwordexportfilters.la
#
######## Files
#
#kde_module_LTLIBRARIES = libhtmlexport.la
#
#libhtmlexport_la_SOURCES = htmlexport.cc ExportDialog.cc ExportDialogUI.ui \
#ExportFilter.cc ExportCss.cc ExportDocStruct.cc ExportBasic.cc
#
#noinst_HEADERS = htmlexport.h ExportDialog.h ExportDialogUI.h ExportFilter.h \
#ExportCss.h ExportDocStruct.h ExportBasic.h
#
#METASOURCES = AUTO
#
#service_DATA = kword_html_export.desktop
#servicedir = $(kde_servicesdir)
