kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/core ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDES}  )


########### next target ###############

set(krosskspreadcore_PART_SRCS 
   kspreadcoremodule.cpp 
   krs_doc.cpp 
   krs_sheet.cpp 
   krs_cell.cpp 
   krs_color.cpp )

kde4_automoc(${krosskspreadcore_PART_SRCS})

kde4_add_plugin(krosskspreadcore ${krosskspreadcore_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} krosskspreadcore )

target_link_libraries(krosskspreadcore  ${KDE4_KDECORE_LIBS} krossapi krossmain kspreadcommon )

install_targets(${PLUGIN_INSTALL_DIR} krosskspreadcore )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/core \
#	-I$(top_srcdir)/kspread $(KOFFICECORE_INCLUDES) $(KSTORE_INCLUDES) \
#	$(KOFFICEUI_INCLUDES) $(KROSS_INCLUDES) \
#        $(all_includes)
#
#kde_module_LTLIBRARIES = krosskspreadcore.la
#
#krosskspreadcore_la_SOURCES = kspreadcoremodule.cpp krs_doc.cpp krs_sheet.cpp krs_cell.cpp krs_color.cpp
#
#krosskspreadcore_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#krosskspreadcore_la_LIBADD = \
#    $(LIB_QT) \
#    $(LIB_KDECORE) \
#    $(top_builddir)/lib/kross/api/libkrossapi.la \
#    $(top_builddir)/lib/kross/main/libkrossmain.la \
#    $(top_builddir)/kspread/libkspreadcommon.la
#
#METASOURCES = AUTO
#SUBDIRS = .
#
#KDE_CXXFLAGS = $(USE_EXCEPTIONS)
#
#noinst_HEADERS = krs_doc.h krs_sheet.h krs_cell.h krs_color.h
