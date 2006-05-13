




########### next target ###############

set(kfile_abiword_PART_SRCS kfile_abiword.cpp )

kde4_automoc(${kfile_abiword_PART_SRCS})

kde4_add_plugin(kfile_abiword ${kfile_abiword_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_abiword )

target_link_libraries(kfile_abiword  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${PLUGIN_INSTALL_DIR} kfile_abiword )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kfile_abiword.desktop )





#original Makefile.am contents follow:

### Makefile.am for gnumeric file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(KOFFICE_INCLUDES) $(all_includes)
#
## these are the headers for your project
#noinst_HEADERS   = kfile_abiword.h
#
#kde_module_LTLIBRARIES = kfile_abiword.la
#
#kfile_abiword_la_SOURCES = kfile_abiword.cpp
#kfile_abiword_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_abiword_la_LIBADD = $(LIB_KOFFICECORE)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#services_DATA = kfile_abiword.desktop
#servicesdir = $(kde_servicesdir)
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_abiword.pot
