kde4_header()

include_directories( ${KOFFICEUI_INCLUDES} ${KDE4_INCLUDES}  )


########### next target ###############

set(kspreadinsertcalendar_PART_SRCS kspread_plugininsertcalendar.cc kspread_insertcalendardialog.cc )

kde4_automoc(${kspreadinsertcalendar_PART_SRCS})

kde4_add_ui_files(kspreadinsertcalendar_PART_SRCS kspread_insertcalendardialogbase.ui )

kde4_add_plugin(kspreadinsertcalendar WITH_PREFIX ${kspreadinsertcalendar_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kspreadinsertcalendar )

target_link_libraries(kspreadinsertcalendar  ${KDE4_KDECORE_LIBS} kspreadcommon )

install_targets(${PLUGIN_INSTALL_DIR} kspreadinsertcalendar )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kspread/kpartplugins FILES  insertcalendar.rc )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES  = -I$(srcdir)/../../../kspread $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) -I$(top_srcdir)/lib/koscript -I$(top_builddir)/lib/koscript $(all_includes)
#
#kde_module_LTLIBRARIES = libkspreadinsertcalendar.la
#
#libkspreadinsertcalendar_la_SOURCES = kspread_plugininsertcalendar.cc \
#				kspread_insertcalendardialogbase.ui \
#				kspread_insertcalendardialog.cc
#libkspreadinsertcalendar_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined
#libkspreadinsertcalendar_la_LIBADD = ../../libkspreadcommon.la
#
#METASOURCES = AUTO
#
## Delete old unused file
##install-data-local: uninstall.desktop
##	$(mkinstalldirs) $(DESTDIR)$(kde_servicesdir)
##	$(INSTALL_DATA) $(srcdir)/uninstall.desktop $(DESTDIR)$(kde_servicesdir)/kspreadinsertcalendar.desktop
#
#rc_DATA = insertcalendar.rc
#rcdir = $(kde_datadir)/kspread/kpartplugins
#
#
#
#SUBDIRS =
#
#messages: rc.cpp
#	$(XGETTEXT) rc.cpp *.cc -o $(podir)/kspreadinsertcalendar.pot
