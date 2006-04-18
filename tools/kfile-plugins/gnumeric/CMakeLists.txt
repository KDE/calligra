kde4_header()




########### next target ###############

set(kfile_gnumeric_PART_SRCS kfile_gnumeric.cpp )

kde4_automoc(${kfile_gnumeric_PART_SRCS})

kde4_add_plugin(kfile_gnumeric ${kfile_gnumeric_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_gnumeric )

target_link_libraries(kfile_gnumeric  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${PLUGIN_INSTALL_DIR} kfile_gnumeric )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kfile_gnumeric.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for gnumeric file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(KOFFICE_INCLUDES) $(all_includes)
#
## these are the headers for your project
#noinst_HEADERS   = kfile_gnumeric.h
#
#kde_module_LTLIBRARIES = kfile_gnumeric.la
#
#kfile_gnumeric_la_SOURCES = kfile_gnumeric.cpp
#kfile_gnumeric_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_gnumeric_la_LIBADD = $(LIB_KOFFICECORE)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#services_DATA = kfile_gnumeric.desktop
#servicesdir = $(kde_servicesdir)
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_gnumeric.pot
