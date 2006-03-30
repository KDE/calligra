kde4_header()




########### next target ###############

set(kfopngexport_PART_SRCS pngexport.cc pngexportdia.cc )

kde4_automoc(${kfopngexport_PART_SRCS})

kde4_add_plugin(kfopngexport WITH_PREFIX ${kfopngexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfopngexport )

target_link_libraries(kfopngexport  ${KDE4_KDEUI_LIBS} kformulalib )

install_targets(${LIB_INSTALL_DIR}/kde4 kfopngexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kformula_png_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(all_includes)
#libkfopngexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libkfopngexport_la_LIBADD    = $(KOFFICE_LIBS) $(LIB_KFORMULA)
#
######## Files
#
#kde_module_LTLIBRARIES = libkfopngexport.la
#
#libkfopngexport_la_SOURCES = pngexport.cc pngexportdia.cc
#
#noinst_HEADERS = pngexport.h pngexportdia.h
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
#service_DATA = kformula_png_export.desktop
#servicedir = $(kde_servicesdir)
