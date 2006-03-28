kde4_header()

add_subdirectory( pics )

include_directories( ${CMAKE_SOURCE_DIR}/lib/koscript ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kspreadcalc_PART_SRCS 
   configdlg.cpp 
   dlabel.cpp 
   kcalc.cpp 
   kcalc_core.cpp 
   main.cpp 
   stats.cpp )

kde4_automoc(${kspreadcalc_PART_SRCS})

kde4_add_plugin(kspreadcalc WITH_PREFIX ${kspreadcalc_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kspreadcalc )

target_link_libraries(kspreadcalc  ${KDE4_KDECORE_LIBS} kspreadcommon )

install_targets(${LIB_INSTALL_DIR}/kde4 kspreadcalc )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kspread/kpartplugins FILES  kspreadcalc.rc )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES  = -I$(srcdir)/../../../kspread $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) -I$(top_srcdir)/lib/koscript -I$(top_builddir)/lib/koscript $(all_includes)
#
#kde_module_LTLIBRARIES = libkspreadcalc.la
#
#libkspreadcalc_la_SOURCES = configdlg.cpp \
#			dlabel.cpp \
#			kcalc.cpp \
#			kcalc_core.cpp \
#			main.cpp \
#			stats.cpp
#libkspreadcalc_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined
#libkspreadcalc_la_LIBADD = ../../libkspreadcommon.la
#
#METASOURCES = AUTO
#
## Delete old unused file
#install-data-local: uninstall.desktop
#	$(mkinstalldirs) $(DESTDIR)$(kde_servicesdir)
#	$(INSTALL_DATA) $(srcdir)/uninstall.desktop $(DESTDIR)$(kde_servicesdir)/kspreadcalc.desktop
#
#rc_DATA = kspreadcalc.rc
#rcdir = $(kde_datadir)/kspread/kpartplugins
#
#SUBDIRS = pics
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/kspreadcalc_calc.pot
#
#
