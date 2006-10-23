




########### next target ###############

set(latexgenerator_PART_SRCS 
   body.cc 
   document.cc 
   kwordgenerator.cc 
   latex.cc 
   paragraph.cc )

kde4_automoc(${latexgenerator_PART_SRCS})

kde4_add_plugin(latexgenerator WITH_PREFIX ${latexgenerator_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} latexgenerator )

target_link_libraries(latexgenerator  ${KDE4_KDECORE_LIBS} )

install(TARGETS latexgenerator  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############






#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) -I$(srcdir)/../parser -I../parser $(all_includes)
#liblatexgenerator_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#liblatexgenerator_la_LIBADD  = -lm $(KOFFICE_LIBS)
#
######## Files
#
#kde_module_LTLIBRARIES = liblatexgenerator.la
#
#liblatexgenerator_la_SOURCES = body.cc document.cc kwordgenerator.cc latex.cc paragraph.cc
#
#noinst_HEADERS = body.h document.h kwordgenerator.h latex.h paragraph.h
#
