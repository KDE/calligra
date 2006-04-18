kde4_header()

add_subdirectory( libqpro )

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(qproimport_PART_SRCS qproimport.cc qproformula.cc )

kde4_automoc(${qproimport_PART_SRCS})

kde4_add_plugin(qproimport WITH_PREFIX ${qproimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} qproimport )

target_link_libraries(qproimport  ${KDE4_KDECORE_LIBS} kspreadcommon qpro )

install_targets(${PLUGIN_INSTALL_DIR} qproimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kspread_qpro_import.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) -I$(srcdir)/libqpro -I$(top_srcdir)/kspread \
#	$(KOFFICE_INCLUDES) \
#	$(KOTEXT_INCLUDES) $(all_includes)
#
######## Files
#
#SUBDIRS = libqpro
#
#kde_module_LTLIBRARIES = libqproimport.la
#
#libqproimport_la_SOURCES = qproimport.cc qproformula.cc
#
#libqproimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#libqproimport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la \
#	libqpro/src/libqpro.la
#
#METASOURCES = AUTO
#
#service_DATA = kspread_qpro_import.desktop
#servicedir = $(kde_servicesdir)
