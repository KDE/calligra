
project(kchart)

add_subdirectory( kdchart )
add_subdirectory( toolbar )
add_subdirectory( pics )
add_subdirectory( templates )

include_directories(
	${CMAKE_SOURCE_DIR}/kchart/kdchart/ 
	${CMAKE_SOURCE_DIR}/interfaces 
	${KOFFICEUI_INCLUDES} )


########### next target ###############

set(kchartcommon_LIB_SRCS 
   kchart_part.cc 
   kchart_view.cc 
   KChartViewAdaptor.cc 
   kchart_factory.cc 
   kchartColorConfigPage.cc 
   kchartParameterConfigPage.cc 
   kchartParameter3dConfigPage.cc 
   kchartBackgroundPixmapConfigPage.cc 
   kchartSubTypeChartPage.cc 
   kchartComboConfigPage.cc 
   kchartFontConfigPage.cc 
   kchartDataConfigPage.cc 
   kchartPieConfigPage.cc 
   kchartParameterPieConfigPage.cc 
   kchartConfigDialog.cc 
   kchartWizard.cc 
   kchartWizardLabelsLegendPage.cc 
   kchartWizardSelectChartSubTypePage.cc 
   kchartWizardSelectChartTypePage.cc 
   kchartWizardSelectDataPage.cc 
   kchartWizardSetupAxesPage.cc 
   kchartWizardSetupDataPage.cc 
   kchartWizardSelectDataFormatPage.cc 
   kchartDataEditor.cc 
   kchartLegendConfigPage.cc 
   kchart_params.cc 
   KChartParamsAdaptor.cc 
   kchartHeaderFooterConfigPage.cc 
   kchartLine3dConfigPage.cc 
   kchartParameterPolarConfigPage.cc 
   kchartPageLayout.cc 
   kchartPrinterDlg.cc 
   csvimportdialog.cc )

kde4_automoc(${kchartcommon_LIB_SRCS})

kde4_add_ui3_files(kchartcommon_LIB_SRCS csvimportdialogui.ui )

# kde4_add_dcop_skels(kchartcommon_LIB_SRCS KChartViewIface.h KChartParamsIface.h )

kde4_add_library(kchartcommon SHARED ${kchartcommon_LIB_SRCS})

target_link_libraries(kchartcommon  ${KDE4_KDE3SUPPORT_LIBS} kochart kofficeui kdchart )

set_target_properties(kchartcommon PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kchartcommon  DESTINATION ${LIB_INSTALL_DIR})


########### next target ###############

set(kchartpart_PART_SRCS kchart_factory_init.cc )

kde4_automoc(${kchartpart_PART_SRCS})

kde4_add_plugin(kchartpart WITH_PREFIX ${kchartpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kchartpart )

target_link_libraries(kchartpart  ${KDE4_KDECORE_LIBS} kchartcommon )

install(TARGETS kchartpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kchart_KDEINIT_SRCS main.cc )

kde4_automoc(${kchart_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kchart ${kchart_KDEINIT_SRCS})

target_link_libraries(kdeinit_kchart  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kdeinit_kchart  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kchart kdeinit_kchart )
install(TARGETS kchart  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  kchartpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kchart.rc kchart_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/kchart)
install( FILES  kchart.desktop  DESTINATION ${XDG_APPS_DIR})







#original Makefile.am contents follow:

#INCLUDES  = $(KOFFICE_INCLUDES) -I$(srcdir)/kdchart -I$(top_srcdir)/interfaces -I$(interfacedir) $(all_includes)
#
### The lib with most of the code (shared with the kchart filters)
#lib_LTLIBRARIES = libkchartcommon.la
#
#libkchartcommon_la_SOURCES = kchart_part.cc \
#	kchart_view.cc kchart_factory.cc \
#	kchartColorConfigPage.cc \
#	kchartParameterConfigPage.cc \
#	kchartParameter3dConfigPage.cc \
#	kchartBackgroundPixmapConfigPage.cc	\
#	kchartSubTypeChartPage.cc	\
#	kchartComboConfigPage.cc \
#	kchartFontConfigPage.cc \
#	kchartDataConfigPage.cc \
#	kchartPieConfigPage.cc \
#	kchartParameterPieConfigPage.cc \
#	kchartConfigDialog.cc \
#	kchartWizard.cc \
#	kchartWizardLabelsLegendPage.cc \
#	kchartWizardSelectChartSubTypePage.cc \
#	kchartWizardSelectChartTypePage.cc \
#	kchartWizardSelectDataPage.cc \
#	kchartWizardSetupAxesPage.cc \
#	kchartWizardSetupDataPage.cc \
#        kchartWizardSelectDataFormatPage.cc \
#	kchartDataEditor.cc \
#	KChartViewIface.skel KChartViewIface.cc \
#	kchartLegendConfigPage.cc kchart_params.cc KChartParamsIface.skel \
#	KChartParamsIface.cc kchartHeaderFooterConfigPage.cc \
#	kchartLine3dConfigPage.cc kchartParameterPolarConfigPage.cc kchartPageLayout.cc \
#	kchartPrinterDlg.cc \
#        csvimportdialog.cc csvimportdialogui.ui
#
#libkchartcommon_la_LIBADD = $(top_builddir)/interfaces/libkochart.la \
#                            $(LIB_KOFFICEUI) \
#                            kdchart/libkdchart.la
#libkchartcommon_la_LDFLAGS = $(KDE_RPATH) -version-info 2:0:0 -no-undefined
#
### The part
#kde_module_LTLIBRARIES = libkchartpart.la
#libkchartpart_la_SOURCES = kchart_factory_init.cc
#libkchartpart_la_LDFLAGS = $(KDE_PLUGIN)
#libkchartpart_la_LIBADD = libkchartcommon.la
#
### The kdeinit loadable module and executable
#kdeinit_LTLIBRARIES = kchart.la
#bin_PROGRAMS =
#kchart_la_SOURCES = main.cc
#kchart_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kchart_la_LIBADD = $(LIB_KOFFICECORE)
#
#noinst_HEADERS =  kchart_part.h \
#		kchart_view.h kchart_factory.h \
#		kchartColorConfigPage.h \
#		kchartParameterConfigPage.h \
#		kchartParameter3dConfigPage.h \
#		kchartBackgroundPixmapConfigPage.h	\
#		kchartSubTypeChartPage.h	\
#		kchartComboConfigPage.h \
#		kchartParameterPieConfigPage.h \
#		kchartFontConfigPage.h \
#		kchartDataConfigPage.h \
#		kchartPieConfigPage.h \
#		kchartConfigDialog.h \
#		kchartWizard.h \
#		kchartWizardLabelsLegendPage.h \
#		kchartWizardSelectChartSubTypePage.h \
#		kchartWizardSelectChartTypePage.h \
#		kchartWizardSelectDataPage.h \
#		kchartWizardSetupAxesPage.h \
#		kchartWizardSetupDataPage.h \
#                kchartWizardSelectDataFormatPage.h \
#		kchartDataEditor.h \
#		kchart_aboutdata.h	\
#		kchartcolorarray.h KChartViewIface.h \
#		kchartLegendConfigPage.h  kchart_params.h \
#		KChartParamsIface.h kchartHeaderFooterConfigPage.h \
#		kchartLine3dConfigPage.h kchartParameterPolarConfigPage.h \
#		kchartPageLayout.h \
#		kchartPrinterDlg.h \
#		csvimportdialog.h
#
#METASOURCES = AUTO
#
#rcdir = $(kde_datadir)/kchart
#rc_DATA = kchart.rc kchart_readonly.rc
#
#kdelnk_DATA = kchart.desktop
#kdelnkdir = $(kde_appsdir)/Office
#
#SUBDIRS =  kdchart . toolbar pics templates
#
#messages: rc.cpp
#	$(XGETTEXT) rc.cpp *.cc kchart_aboutdata.h kdchart/*.cpp -o $(podir)/kchart.pot
#
#kde_services_DATA = kchartpart.desktop
#
#include $(top_srcdir)/admin/Doxyfile.am
