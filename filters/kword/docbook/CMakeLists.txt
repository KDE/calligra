kde4_header()




########### next target ###############

set(docbookexport_PART_SRCS docbookexport.cc )

kde4_automoc(${docbookexport_PART_SRCS})

kde4_add_plugin(docbookexport WITH_PREFIX ${docbookexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} docbookexport )

target_link_libraries(docbookexport  ${KDE4_KDECORE_LIBS} kwordexportfilters)

install_targets(${LIB_INSTALL_DIR}/kde4 docbookexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_docbook_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)
#libdocbookexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libdocbookexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la
#
######## Files
#
#kde_module_LTLIBRARIES = libdocbookexport.la
#
#libdocbookexport_la_SOURCES = docbookexport.cc
#
#noinst_HEADERS = docbookexport.h 
#
#METASOURCES = AUTO
#
#service_DATA = kword_docbook_export.desktop
#servicedir = $(kde_servicesdir)
