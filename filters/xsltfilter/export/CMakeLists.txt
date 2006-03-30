kde4_header()

add_subdirectory( xsl )

include_directories(${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR})



########### next target ###############

set(xsltexport_PART_SRCS xsltexport.cc xsltexportdia.cc xsltproc.cc )

kde4_automoc(${xsltexport_PART_SRCS})

kde4_add_ui3_files(xsltexport_PART_SRCS xsltdialog.ui )

kde4_add_plugin(xsltexport WITH_PREFIX ${xsltexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} xsltexport )

target_link_libraries(xsltexport  ${KDE4_KIO_LIBS} kofficecore ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} )

install_targets(${LIB_INSTALL_DIR}/kde4 xsltexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  xslt_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#SUBDIRS = xsl
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes) $(LIBXML_CFLAGS) $(LIBXSLT_CFLAGS)
#
######## Files
#
#kde_module_LTLIBRARIES = libxsltexport.la
#
#libxsltexport_la_SOURCES = xsltexport.cc xsltexportdia.cc xsltproc.cc xsltdialog.ui
#libxsltexport_la_LDFLAGS = $(KDE_RPATH) $(LIBXML_RPATH) $(LIBXSLT_RPATH) $(all_libraries) -module $(KDE_PLUGIN)
#libxsltexport_la_LIBADD	= $(KOFFICE_LIBS) $(LIBXSLT_LIBS) $(LIBXML_LIBS)
#
#noinst_HEADERS = xsltexport.h xsltexportdia.h xsltproc.h
#
######### Debug
##check_PROGRAMS = xsltfilter
##xsltfilter_SOURCES = xsltfilter.cc
##xsltfilter_LDADD = $(LIBXSLT_LIBS) $(LIBXML_LIBS)
##xsltfilter_LDFLAGS = $(KDE_RPATH) $(LIBXML_RPATH) $(LIBXSLT_RPATH) $(all_libraries)
#
#
#METASOURCES = AUTO
#
#service_DATA = xslt_export.desktop
#
#servicedir = $(kde_servicesdir)
