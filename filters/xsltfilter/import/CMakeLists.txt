kde4_header()

include_directories(${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR})


########### next target ###############

set(xsltimport_PART_SRCS xsltimport.cc xsltimportdia.cc xsltproc.cc )

kde4_automoc(${xsltimport_PART_SRCS})

kde4_add_ui3_files(xsltimport_PART_SRCS xsltdialog.ui )

kde4_add_plugin(xsltimport WITH_PREFIX ${xsltimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} xsltimport )

target_link_libraries(xsltimport  ${KDE4_KIO_LIBS} kofficecore ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} )

install_targets(${LIB_INSTALL_DIR}/kde4 xsltimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  xslt_import.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES)  $(all_includes) $(LIBXML_CFLAGS) $(LIBXSLT_CFLAGS)
#
######## Files
#
#kde_module_LTLIBRARIES = libxsltimport.la
#
#libxsltimport_la_SOURCES = xsltimport.cc xsltimportdia.cc xsltproc.cc xsltdialog.ui
#libxsltimport_la_LDFLAGS = $(KDE_RPATH) $(LIBXML_RPATH) $(LIBXSLT_RPATH) $(all_libraries) -module $(KDE_PLUGIN)
#libxsltimport_la_LIBADD	= $(KOFFICE_LIBS) $(LIBXSLT_LIBS) $(LIBXML_LIBS)
#
#noinst_HEADERS = xsltimport.h xsltimportdia.h xsltproc.h
#
######### Debug
##check_PROGRAMS = xsltfilter
##xsltfilter_SOURCES = xsltfilter.c
##xsltfilter_LDADD = $(LIBXSLT_LIBS) $(LIBXML_LIBS)
##xsltfilter_LDFLAGS = $(KDE_RPATH) $(LIBXML_RPATH) $(LIBXSLT_RPATH) $(all_libraries)
#
#METASOURCES = AUTO
#
#service_DATA = xslt_import.desktop
#servicedir = $(kde_servicesdir)
