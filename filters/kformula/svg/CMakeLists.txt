kde4_header()




########### next target ###############

set(kfosvgexport_PART_SRCS svgexport.cc )

kde4_automoc(${kfosvgexport_PART_SRCS})

kde4_add_plugin(kfosvgexport WITH_PREFIX ${kfosvgexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfosvgexport )

target_link_libraries(kfosvgexport  ${KDE4_KDECORE_LIBS} kofficecore kformulalib )

install_targets(${LIB_INSTALL_DIR}/kde4 kfosvgexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kformula_svg_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(all_includes)
#libkfosvgexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libkfosvgexport_la_LIBADD    = $(KOFFICE_LIBS) $(LIB_KFORMULA)
#
######## Files
#
#kde_module_LTLIBRARIES = libkfosvgexport.la
#
#libkfosvgexport_la_SOURCES = svgexport.cc
#
#noinst_HEADERS = svgexport.h
#
######### Debug
##check_PROGRAMS = texlauncher
##texlauncher_SOURCES = texlauncher.cc
##texlauncher_LDADD = liblatexexport.la
##texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#
#
#METASOURCES = AUTO
#
#service_DATA = kformula_svg_export.desktop
#servicedir = $(kde_servicesdir)
