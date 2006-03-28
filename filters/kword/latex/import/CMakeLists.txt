kde4_header()

add_subdirectory( parser )
add_subdirectory( generator )




########### next target ###############

set(kwordlateximport_PART_SRCS config.cc lateximport.cc lateximportdia.cc )

kde4_automoc(${kwordlateximport_PART_SRCS})

kde4_add_dcop_skels(kwordlateximport_PART_SRCS lateximportdia.h )

kde4_add_plugin(kwordlateximport WITH_PREFIX ${kwordlateximport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwordlateximport )

target_link_libraries(kwordlateximport  ${KDE4_KDECORE_LIBS} )

install_targets(${LIB_INSTALL_DIR}/kde4 kwordlateximport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_latex_import.desktop )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = parser generator
#
######## General stuff
#
#INCLUDES= -I$(srcdir) -I$(srcdir)/parser -I$(srcdir)/generator $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(all_includes)
#libkwordlateximport_la_LDFLAGS= $(all_libraries) -module -avoid-version -no-undefined
#libkwordlateximport_la_LIBADD = $(KOFFICE_LIBS) parser/liblatexparser.la generator/liblatexgenerator.la
#
######## Files
#
#kde_module_LTLIBRARIES = libkwordlateximport.la
#
#libkwordlateximport_la_SOURCES = config.cc lateximport.cc \
#																 lateximportdia.cc lateximportdia.skel
#
#noinst_HEADERS = config.h lateximport.h lateximportdia.h
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
#service_DATA = kword_latex_import.desktop
#servicedir = $(kde_servicesdir)
