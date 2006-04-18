kde4_header()




########### next target ###############

set(kugarnopimport_PART_SRCS kugarnopimport.cpp )

kde4_automoc(${kugarnopimport_PART_SRCS})

kde4_add_plugin(kugarnopimport WITH_PREFIX ${kugarnopimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kugarnopimport )

target_link_libraries(kugarnopimport  ${KDE4_KDECORE_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} kugarnopimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kugar_kugar_import.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES = libkugarnopimport.la
#
#libkugarnopimport_la_SOURCES = kugarnopimport.cpp
#libkugarnopimport_la_LDFLAGS = -module $(KDE_PLUGIN)
#libkugarnopimport_la_LIBADD = $(KOFFICE_LIBS)
#
#noinst_HEADERS = kugarnopimport.h 
#
#METASOURCES = AUTO
#
#service_DATA = kugar_kugar_import.desktop
#servicedir = $(kde_servicesdir)
#
## Note: If your filter imports or exports some special file
## which KDE doesn't have a mimetype for, yet, you'll have to
## create a mimetype and install it using those two lines.
## In case of doubt please ask koffice@kde.org or
## koffice-devel@kde.org. Thanks.
## mydata_DATA = x-foo.desktop
## mydatadir = $(kde_mimedir)/text
