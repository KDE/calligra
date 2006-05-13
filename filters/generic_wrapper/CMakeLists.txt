

include_directories(
    ${KOFFICECORE_INCLUDES}
    )




########### next target ###############

set(generickofilter_PART_SRCS generic_filter.cc )

kde4_automoc(${generickofilter_PART_SRCS})

kde4_add_plugin(generickofilter WITH_PREFIX ${generickofilter_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} generickofilter )

target_link_libraries(generickofilter  ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} kofficecore )

install_targets(${PLUGIN_INSTALL_DIR} generickofilter )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  generic_filter.desktop )





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#
######## Files
#kde_module_LTLIBRARIES = libgenerickofilter.la 
#
#
#libgenerickofilter_la_SOURCES = generic_filter.cc
#libgenerickofilter_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#libgenerickofilter_la_LIBADD  = $(KOFFICE_LIBS) $(LIB_KIO)
#
#METASOURCES = AUTO
#
#noinst_HEADERS = generic_filter.h
#
#service_DATA = generic_filter.desktop
#servicedir = $(kde_servicesdir)
