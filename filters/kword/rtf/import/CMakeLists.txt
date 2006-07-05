




########### next target ###############

set(rtfimport_PART_SRCS rtfimport.cpp rtfimport_dom.cpp rtfimport_tokenizer.cpp )

kde4_automoc(${rtfimport_PART_SRCS})

kde4_add_plugin(rtfimport WITH_PREFIX ${rtfimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} rtfimport )

target_link_libraries(rtfimport kofficecore)

install(TARGETS rtfimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_rtf_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= $(KOFFICE_INCLUDES) $(all_includes)
#librtfimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#librtfimport_la_LIBADD = $(KOFFICE_LIBS)
#
######## Files
#
#kde_module_LTLIBRARIES = librtfimport.la
#
#librtfimport_la_SOURCES = rtfimport.cpp rtfimport_dom.cpp rtfimport_tokenizer.cpp
#
#noinst_HEADERS = rtfimport.h rtfimport_dom.h rtfimport_tokenizer.h
#
#METASOURCES = AUTO
#
#service_DATA = kword_rtf_import.desktop
#servicedir = $(kde_servicesdir)
