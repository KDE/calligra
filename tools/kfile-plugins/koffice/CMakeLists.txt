




########### next target ###############

set(kfile_koffice_PART_SRCS kfile_koffice.cpp )

kde4_automoc(${kfile_koffice_PART_SRCS})

kde4_add_plugin(kfile_koffice ${kfile_koffice_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_koffice )

target_link_libraries(kfile_koffice  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kfile_koffice  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kfile_koffice.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

### Makefile.am for koffice file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(KOFFICE_INCLUDES) $(all_includes)
#
## these are the headers for your project
#noinst_HEADERS   = kfile_koffice.h
#
#kde_module_LTLIBRARIES = kfile_koffice.la
#
#kfile_koffice_la_SOURCES = kfile_koffice.cpp
#kfile_koffice_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_koffice_la_LIBADD = $(LIB_KOFFICECORE)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#services_DATA = kfile_koffice.desktop
#servicesdir = $(kde_servicesdir)
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_koffice.pot
