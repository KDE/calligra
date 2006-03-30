kde4_header()




########### next target ###############

set(kfolatexexport_PART_SRCS latexexport.cc )

kde4_automoc(${kfolatexexport_PART_SRCS})

kde4_add_plugin(kfolatexexport WITH_PREFIX ${kfolatexexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfolatexexport )

target_link_libraries(kfolatexexport  ${KDE4_KDECORE_LIBS} kofficecore kformulalib )

install_targets(${LIB_INSTALL_DIR}/kde4 kfolatexexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kformula_latex_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(all_includes)
#libkfolatexexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libkfolatexexport_la_LIBADD    = $(KOFFICE_LIBS) $(LIB_KFORMULA)
#
######## Files
#
#kde_module_LTLIBRARIES = libkfolatexexport.la
#
#libkfolatexexport_la_SOURCES = latexexport.cc
#
#noinst_HEADERS = latexexport.h
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
#service_DATA = kformula_latex_export.desktop
#servicedir = $(kde_servicesdir)
