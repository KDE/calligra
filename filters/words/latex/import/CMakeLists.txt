

add_subdirectory( parser )
add_subdirectory( generator )




########### next target ###############

set(wordslateximport_PART_SRCS config.cc lateximport.cc lateximportdia.cc )

kde4_add_plugin(wordslateximport ${wordslateximport_PART_SRCS})



target_link_libraries(wordslateximport  ${KDE4_KDECORE_LIBS} )

install(TARGETS wordslateximport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  words_latex_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#SUBDIRS = parser generator
#
######## General stuff
#
#INCLUDES= -I$(srcdir) -I$(srcdir)/parser -I$(srcdir)/generator $(CALLIGRA_INCLUDES) $(KFORMULA_INCLUDES) $(all_includes)
#libwordslateximport_la_LDFLAGS= $(all_libraries) -module -avoid-version -no-undefined
#libwordslateximport_la_LIBADD = $(CALLIGRA_LIBS) parser/liblatexparser.la generator/liblatexgenerator.la
#
######## Files
#
#kde_module_LTLIBRARIES = libwordslateximport.la
#
#libwordslateximport_la_SOURCES = config.cc lateximport.cc \
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
#service_DATA = words_latex_import.desktop
#servicedir = $(kde_servicesdir)
