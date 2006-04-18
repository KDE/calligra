kde4_header()




########### next target ###############

set(clipartthumbnail_PART_SRCS clipartcreator.cpp )

kde4_automoc(${clipartthumbnail_PART_SRCS})

kde4_add_plugin(clipartthumbnail ${clipartthumbnail_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} clipartthumbnail )

target_link_libraries(clipartthumbnail  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${PLUGIN_INSTALL_DIR} clipartthumbnail )


########### next target ###############

set(kofficethumbnail_PART_SRCS kofficecreator.cpp )

kde4_automoc(${kofficethumbnail_PART_SRCS})

kde4_add_plugin(kofficethumbnail ${kofficethumbnail_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kofficethumbnail )

target_link_libraries(kofficethumbnail  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${PLUGIN_INSTALL_DIR} kofficethumbnail )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  clipartthumbnail.desktop kofficethumbnail.desktop )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) -I../../kword/ -I../../kpresenter -I../../lib/kotext $(all_includes)
#METASOURCES = AUTO
#
#kde_module_LTLIBRARIES = clipartthumbnail.la kofficethumbnail.la
#
#clipartthumbnail_la_SOURCES = clipartcreator.cpp
#clipartthumbnail_la_LIBADD = $(LIB_KOFFICECORE) # for KoClipartCollection
#clipartthumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
#kofficethumbnail_la_SOURCES = kofficecreator.cpp
#kofficethumbnail_la_LIBADD = $(LIB_KOFFICECORE)
#kofficethumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
## otherofficethumbnail.desktop is currently not installed, as it gives too many problems
#kde_services_DATA = clipartthumbnail.desktop kofficethumbnail.desktop
#
