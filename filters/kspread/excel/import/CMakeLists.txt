kde4_header()

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder )


########### next target ###############

set(sidewinder_SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/cell.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/excel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/format.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/pole.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/sheet.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/ustring.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/value.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/workbook.cpp
)

set(excelimport_PART_SRCS excelimport.cc ${sidewinder_SRCS})

kde4_automoc(${excelimport_PART_SRCS})

kde4_add_plugin(excelimport WITH_PREFIX ${excelimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} excelimport )

target_link_libraries(excelimport kofficecore)

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
