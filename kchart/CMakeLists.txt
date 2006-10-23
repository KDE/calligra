
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
)

kde4_automoc(${kchartcommon_LIB_SRCS})

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


