kde4_header()

add_subdirectory( xpdf )




########### next target ###############

set(pdfimport_PART_SRCS 
   misc.cpp 
   transform.cpp 
   data.cpp 
   fstring.cpp 
   FilterPage.cpp 
   FilterDevice.cpp 
   pdfdocument.cpp 
   dialog.cpp 
   pdfimport.cpp )

kde4_automoc(${pdfimport_PART_SRCS})

kde4_add_plugin(pdfimport WITH_PREFIX ${pdfimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} pdfimport )

target_link_libraries(pdfimport  ${KDE4_KDECORE_LIBS} kofficecore xpdf goo )

install_targets(${PLUGIN_INSTALL_DIR} pdfimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_pdf_import.desktop )

kde4_footer()



#original Makefile.am contents follow:

#KDE_OPTIONS = nofinal
#
#SUBDIRS = xpdf
#
#INCLUDES= $(KOFFICE_INCLUDES) -I$(srcdir) -I$(srcdir)/xpdf/goo -I$(srcdir)/xpdf/xpdf $(all_includes)
#
#kde_module_LTLIBRARIES = libpdfimport.la
#
#libpdfimport_la_SOURCES = misc.cpp transform.cpp data.cpp fstring.cpp \
#                          FilterPage.cpp FilterDevice.cpp \
#                          pdfdocument.cpp dialog.cpp pdfimport.cpp
#libpdfimport_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
#libpdfimport_la_LIBADD = $(LIB_KOFFICECORE) $(LIBPAPER) ./xpdf/xpdf/libxpdf.la ./xpdf/goo/libgoo.la
#
#METASOURCES = dialog.moc pdfimport.moc
#
#service_DATA = kword_pdf_import.desktop
#servicedir = $(kde_servicesdir)
