kde4_header()




########### next target ###############

set(excelimport_PART_SRCS excelimport.cc )

kde4_automoc(${excelimport_PART_SRCS})

kde4_add_plugin(excelimport WITH_PREFIX ${excelimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} excelimport )

target_link_libraries(excelimport  ${KDE4_KDECORE_LIBS} sidewinder )

install_targets(${PLUGIN_INSTALL_DIR} excelimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kspread_excel_import.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) -I$(srcdir)/../sidewinder $(KOFFICE_INCLUDES) $(all_includes)
#
######## Files
#
#kde_module_LTLIBRARIES = libexcelimport.la
#
#libexcelimport_la_SOURCES = excelimport.cc 
#libexcelimport_la_LIBADD = $(KOFFICE_LIBS) ../sidewinder/libsidewinder.la
#libexcelimport_la_LDFLAGS = -module $(KDE_PLUGIN)
#noinst_HEADERS = excelimport.h
#
#METASOURCES = AUTO
#
#service_DATA = kspread_excel_import.desktop
#servicedir = $(kde_servicesdir)
