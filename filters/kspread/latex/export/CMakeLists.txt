




########### next target ###############

set(kspreadlatexexport_PART_SRCS 
   cell.cc 
   column.cc 
   config.cc 
   document.cc 
   fileheader.cc 
   format.cc 
   kspreadlatexexportdiaImpl.cc 
   latexexport.cc 
   latexexportAdaptor.cc 
   map.cc 
   pen.cc 
   row.cc 
   spreadsheet.cc 
   table.cc 
   xmlparser.cc )

kde4_automoc(${kspreadlatexexport_PART_SRCS})

kde4_add_ui3_files(kspreadlatexexport_PART_SRCS latexexportdia.ui )

kde4_add_plugin(kspreadlatexexport WITH_PREFIX ${kspreadlatexexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kspreadlatexexport )

target_link_libraries(kspreadlatexexport  ${KDE4_KDEUI_LIBS} kstore kofficeui )

install(TARGETS kspreadlatexexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_latex_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(all_includes)
#libkspreadlatexexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libkspreadlatexexport_la_LIBADD    = $(KOFFICE_LIBS) $(LIB_KFORMULA)
#
######## Files
#
#kde_module_LTLIBRARIES = libkspreadlatexexport.la
#
#libkspreadlatexexport_la_SOURCES = cell.cc column.cc config.cc document.cc fileheader.cc \
#				 format.cc latexexportdia.ui kspreadlatexexportdiaImpl.cc \
#				 latexexport.cc latexexportIface.cc latexexportIface.skel \
#				 map.cc pen.cc row.cc spreadsheet.cc table.cc \
#				 xmlparser.cc
#
#noinst_HEADERS = cell.h column.h config.h document.h fileheader.h format.h \
#					kspreadlatexexportdiaImpl.h latexexport.h latexexportIface.h \
#					map.h pen.h row.h spreadsheet.h table.h xmlparser.h
#
######### Debug
##check_PROGRAMS = texlauncher
##texlauncher_SOURCES = texlauncher.cc
##texlauncher_LDADD = liblatexexport.la
##texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#
#METASOURCES = AUTO
#
#service_DATA = kspread_latex_export.desktop
#servicedir = $(kde_servicesdir)
