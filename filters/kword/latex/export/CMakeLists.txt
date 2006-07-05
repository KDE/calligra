

include_directories(${KFORMULA_INCLUDES} ${CMAKE_BINARY_DIR}/filters/)

MESSAGE(STATUS "CMAKE_CURRENT_BINARY_DIR :<${CMAKE_BINARY_DIR}")
########### next target ###############

set(kwordlatexexport_PART_SRCS 
   anchor.cc 
   config.cc 
   document.cc 
   element.cc 
   fileheader.cc 
   footnote.cc 
   format.cc 
   formula.cc 
   key.cc 
   kwordlatexexportdia.cc 
   latexexport.cc 
   latexexportAdaptor.cc 
   layout.cc 
   listtable.cc 
   para.cc 
   pixmapFrame.cc 
   table.cc 
   textFrame.cc 
   textformat.cc 
   textzone.cc 
   variableformat.cc 
   variablezone.cc 
   xml2latexparser.cc 
   xmlparser.cc )

kde4_automoc(${kwordlatexexport_PART_SRCS})

kde4_add_ui3_files(kwordlatexexport_PART_SRCS latexexportdia.ui )


kde4_add_plugin(kwordlatexexport WITH_PREFIX ${kwordlatexexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwordlatexexport )

target_link_libraries(kwordlatexexport  ${KDE4_KIO_LIBS} kofficecore kformulalib ${IMAGEMAGICK_LIBRARIES} )

install(TARGETS kwordlatexexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_latex_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(LIBMAGICK_CPPFLAGS) $(all_includes)
#libkwordlatexexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined $(LIBMAGICK_RPATH) $(LIBMAGICK_LDFLAGS)
#libkwordlatexexport_la_LIBADD  = $(KOFFICE_LIBS) $(LIB_KFORMULA) $(LIBMAGICK_LIBS)
#
######## Files
#
#kde_module_LTLIBRARIES = libkwordlatexexport.la
#
#libkwordlatexexport_la_SOURCES = anchor.cc config.cc document.cc element.cc fileheader.cc \
# latexexportdia.ui footnote.cc format.cc formula.cc key.cc \
# kwordlatexexportdia.cc latexexport.cc latexexportIface.cc \
# latexexportIface.skel layout.cc listtable.cc para.cc \
# pixmapFrame.cc table.cc \
# textFrame.cc textformat.cc textzone.cc variableformat.cc \
# variablezone.cc xml2latexparser.cc xmlparser.cc
#
######### Debug
##check_PROGRAMS = texlauncher
##texlauncher_SOURCES = texlauncher.cc
##texlauncher_LDADD = liblatexexport.la
##texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#
#METASOURCES = AUTO
#
#service_DATA = kword_latex_export.desktop
#servicedir = $(kde_servicesdir)
