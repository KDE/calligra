kde4_header()




########### next target ###############

set(kfile_ooo_PART_SRCS kfile_ooo.cpp )

kde4_automoc(${kfile_ooo_PART_SRCS})

kde4_add_plugin(kfile_ooo ${kfile_ooo_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_ooo )

target_link_libraries(kfile_ooo  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${PLUGIN_INSTALL_DIR} kfile_ooo )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kfile_ooo.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for OpenOffice.org file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(KOFFICE_INCLUDES) $(all_includes)
#
## these are the headers for your project
#noinst_HEADERS   = kfile_ooo.h
#
#kde_module_LTLIBRARIES = kfile_ooo.la
#
#kfile_ooo_la_SOURCES = kfile_ooo.cpp
#kfile_ooo_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_ooo_la_LIBADD = $(LIB_KOFFICECORE)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#services_DATA = kfile_ooo.desktop
#servicesdir = $(kde_servicesdir)
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_ooo.pot
