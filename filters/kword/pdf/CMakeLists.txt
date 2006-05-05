kde4_header()
include_directories(${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf ${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/goo)

set(libxpdf_SRCS
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Array.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/BuiltinFont.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/BuiltinFontTables.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Catalog.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/CharCodeToUnicode.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/CMap.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Decrypt.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Dict.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Error.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/FontEncodingTables.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/FontFile.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Function.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Gfx.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/GfxFont.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/GfxState.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/GlobalParams.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Lexer.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Link.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/NameToCharCode.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Object.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/OutputDev.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Page.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Parser.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/PDFDoc.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/PSTokenizer.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Stream.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/UnicodeMap.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/XRef.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Annot.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/Outline.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/JBIG2Stream.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/PDFDocEncoding.cc 
    ${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/xpdf/TextOutputDev.cc
)

set(libgoo_SRCS
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/goo/GHash.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/goo/GList.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/goo/GString.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/goo/gmempp.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/goo/gfile.cc 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/goo/gmem.c 
	${CMAKE_SOURCE_DIR}/filters/kword/pdf/xpdf/goo/parseargs.c
	)



########### next target ###############

set(pdfimport_PART_SRCS ${libgoo_SRCS} ${libxpdf_SRCS}
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

target_link_libraries(pdfimport  ${KDE4_KDECORE_LIBS} kofficecore )

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
