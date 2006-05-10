kde4_header()


include_directories( ${KDE4_INCLUDES} )

########### next target ###############

set(thesaurustool_PART_SRCS main.cc )

kde4_automoc(${thesaurustool_PART_SRCS})

kde4_add_plugin(thesaurustool WITH_PREFIX ${thesaurustool_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} thesaurustool )

target_link_libraries(thesaurustool  ${KDE4_KIO_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} thesaurustool )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  thesaurustool.desktop )
install_files( ${DATA_INSTALL_DIR}/thesaurus FILES  thesaurus.txt )

kde4_footer()



#original Makefile.am contents follow:

#
#INCLUDES = $(all_includes)
#libthesaurustool_la_LIBADD = $(LIB_KSYCOCA)
#
######## Files
#
#kde_module_LTLIBRARIES = libthesaurustool.la
#
#libthesaurustool_la_SOURCES = main.cc
#
#libthesaurustool_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined
#
#noinst_HEADERS = main.h
#
#libthesaurustool_la_METASOURCES = main.moc
#
#service_DATA = thesaurustool.desktop
#servicedir = $(kde_servicesdir)
#
#thesaurusdir = $(kde_datadir)/thesaurus
#thesaurus_DATA = thesaurus.txt
#
#messages:
#	$(XGETTEXT) *.cc -o $(podir)/thesaurus_tool.pot
