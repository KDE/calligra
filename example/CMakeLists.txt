kde4_header()




########### next target ###############

set(examplepart_PART_SRCS example_part.cc example_view.cc example_factory.cc )

kde4_automoc(${examplepart_PART_SRCS})

kde4_add_plugin(examplepart WITH_PREFIX ${examplepart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} examplepart )

target_link_libraries(examplepart  ${KDE4_KDECORE_LIBS} kofficeui )

install_targets(${LIB_INSTALL_DIR}/kde4 examplepart )


########### next target ###############

set(example_KDEINIT_SRCS main.cc )

kde4_automoc(${example_KDEINIT_SRCS})

kde4_add_kdeinit_executable( example ${example_KDEINIT_SRCS})

target_link_libraries(kdeinit_example  ${KDE4_KDECORE_LIBS} kofficeui )

install_targets(${LIB_INSTALL_DIR} kdeinit_example )

target_link_libraries( example kdeinit_example )
install_targets(/bin example )

########### install files ###############

install_files( ${MIME_INSTALL_DIR}/application FILES  x-vnd.kde.example.desktop )
install_files( ${SERVICES_INSTALL_DIR} FILES  examplepart.desktop )
install_files( ${DATA_INSTALL_DIR}/example FILES  example.rc example_readonly.rc )
install_files( ${APPLNK_INSTALL_DIR}/Office FILES  example.desktop )

kde4_create_doxygen_docs(  )

kde4_footer()



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
