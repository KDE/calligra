
project(kformula)
add_subdirectory( pics )

include_directories(${KFORMULA_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOTEXT_INCLUDES})


########### next target ###############

set(kformulapart_PART_SRCS 
   kformula_doc.cc 
   kformula_view.cc 
   kformula_view_adaptor.cc
   kformula_factory.cc 
   kformulawidget.cc 
   kfconfig.cc 
   formulastring.cc 
   fsparser.cc )

kde4_automoc(${kformulapart_PART_SRCS})

kde4_add_plugin(kformulapart WITH_PREFIX ${kformulapart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kformulapart )

target_link_libraries(kformulapart  ${KDE4_KDECORE_LIBS} kofficeui kformulalib kotext )

install_targets(${PLUGIN_INSTALL_DIR} kformulapart )


########### next target ###############

set(kformula_KDEINIT_SRCS main.cc )

kde4_automoc(${kformula_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kformula ${kformula_KDEINIT_SRCS})

target_link_libraries(kdeinit_kformula  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${LIB_INSTALL_DIR} kdeinit_kformula )

target_link_libraries( kformula kdeinit_kformula )
install_targets(/bin kformula )

########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kformula FILES  tips )
install_files( ${SERVICES_INSTALL_DIR} FILES  kformulapart.desktop )
install_files( ${DATA_INSTALL_DIR}/kformula FILES  kformula.rc kformula_readonly.rc )
install_files( ${XDG_APPS_DIR} FILES  kformula.desktop )







#original Makefile.am contents follow:

#INCLUDES  = $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(KOTEXT_INCLUDES) -I$(interfacedir) $(all_includes)
#KDE_CXXFLAGS = $(USE_RTTI)
#
### The part
#kde_module_LTLIBRARIES = libkformulapart.la
#libkformulapart_la_SOURCES = kformula_doc.cc \
#	kformula_view.cc kformula_factory.cc kformulawidget.cc kfconfig.cc \
#	kformula_view_iface.skel kformula_view_iface.cc formulastring.cc fsparser.cc
#libkformulapart_la_LDFLAGS = $(KDE_PLUGIN)
#libkformulapart_la_LIBADD = $(LIB_KOFFICEUI) $(LIB_KFORMULA) $(LIB_KOTEXT)
#libkformulapart_la_METASOURCES = AUTO
#
### The kdeinit loadable module
#kdeinit_LTLIBRARIES = kformula.la
#kformula_la_SOURCES = main.cc
#kformula_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kformula_la_LIBADD = $(LIB_KOFFICECORE)
#
#bin_PROGRAMS =
#lib_LTLIBRARIES =
#
#noinst_HEADERS =  kformula_doc.h \
#		  kformula_view.h kformula_factory.h \
#		  kformula_aboutdata.h kformula_view_iface.h formulastring.h fsparser.h
#
#kdelnk_DATA = kformula.desktop
#kdelnkdir = $(kde_appsdir)/Office
#
#rc_DATA = kformula.rc kformula_readonly.rc
#rcdir = $(kde_datadir)/kformula
#
#SUBDIRS =  pics
#
#tipdir = $(kde_datadir)/kformula
#tip_DATA = tips
#
#messages: rc.cpp
#	$(PREPARETIPS) >> rc.cpp
#	$(XGETTEXT) rc.cpp *.cc kformula_aboutdata.h -o $(podir)/kformula.pot
#kde_services_DATA = kformulapart.desktop
#
#include $(top_srcdir)/admin/Doxyfile.am
