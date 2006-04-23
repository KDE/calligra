kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(kthesaurus_KDEINIT_SRCS main.cc )

kde4_automoc(${kthesaurus_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kthesaurus ${kthesaurus_KDEINIT_SRCS})

target_link_libraries(kdeinit_kthesaurus  ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} )

install_targets(${LIB_INSTALL_DIR} kdeinit_kthesaurus )

target_link_libraries( kthesaurus kdeinit_kthesaurus )
install_targets(/bin kthesaurus )

########### install files ###############

install_files( ${APPLNK_INSTALL_DIR}/Office FILES  KThesaurus.desktop )

kde4_footer()



#original Makefile.am contents follow:

#
#INCLUDES = $(KOFFICECORE_INCLUDES) $(all_includes)
#
#bin_PROGRAMS =
#lib_LTLIBRARIES =
#kdeinit_LTLIBRARIES = kthesaurus.la
#
#kthesaurus_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module -avoid-version
#kthesaurus_la_SOURCES = main.cc
#kthesaurus_la_LIBADD = $(LIB_KSYCOCA)
# 
#appsdir = $(kde_appsdir)/Office
#apps_DATA = KThesaurus.desktop
#
#messages:
#	$(XGETTEXT) *.cc -o $(podir)/kthesaurus.pot
