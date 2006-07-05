

include_directories( ${KOFFICEUI_INCLUDES} )


########### next target ###############

set(examplepart_PART_SRCS example_part.cc example_view.cc example_factory.cc )

kde4_automoc(${examplepart_PART_SRCS})

kde4_add_plugin(examplepart WITH_PREFIX ${examplepart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} examplepart )

target_link_libraries(examplepart  ${KDE4_KPARTS_LIBS} kofficecore )

install(TARGETS examplepart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############


########### install files ###############

install( FILES  x-vnd.kde.example.desktop  DESTINATION ${MIME_INSTALL_DIR}/application)
install( FILES  examplepart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  example.rc example_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/example)
install( FILES  example.desktop  DESTINATION ${XDG_APPS_DIR})







#original Makefile.am contents follow:

#
#INCLUDES  = $(KOFFICE_INCLUDES) $(all_includes)
#
### The part
#kde_module_LTLIBRARIES = libexamplepart.la
#libexamplepart_la_SOURCES = example_part.cc \
#	example_view.cc example_factory.cc
#libexamplepart_la_LDFLAGS = $(KDE_PLUGIN)
#libexamplepart_la_LIBADD = $(LIB_KOFFICEUI)
#
### The kdeinit loadable module and executable
#kdeinit_LTLIBRARIES = example.la
#lib_LTLIBRARIES =
#bin_PROGRAMS =
#example_la_SOURCES = main.cc
#example_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#example_la_LIBADD = $(LIB_KOFFICEUI)
#
#METASOURCES = AUTO
#
#kdelnk_DATA = example.desktop
#kdelnkdir = $(kde_appsdir)/Office
#
## Note: for your application, the mime type should be defined in
##       kdelibs CVS HEAD and a compatibility entry should be added in
##       the directory koffice/mimetypes/kde??
##       where ?? is the version number of the last stable kde(libs) release.
#kdemime_DATA = x-vnd.kde.example.desktop
#kdemimedir = $(kde_mimedir)/application
#
#rc_DATA = example.rc example_readonly.rc
#rcdir = $(kde_datadir)/example
#
#messages: rc.cpp
#	$(XGETTEXT) rc.cpp *.cc example_aboutdata.h -o $(podir)/example.pot
#kde_services_DATA = examplepart.desktop
#
#include $(top_srcdir)/admin/Doxyfile.am
