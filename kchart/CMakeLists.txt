
project(kchart)

add_subdirectory( toolbar )
add_subdirectory( pics )
add_subdirectory( templates )
add_subdirectory( tests )

include_directories(
    ${KOMAIN_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}/kdchart/include
	${CMAKE_CURRENT_SOURCE_DIR}/kdchart/src
	${CMAKE_CURRENT_SOURCE_DIR}/kdchart/examples/tools
	${CMAKE_CURRENT_SOURCE_DIR}/dialogs
	${CMAKE_CURRENT_SOURCE_DIR}/shape
	${CMAKE_SOURCE_DIR}/interfaces 
	 )


###########  target kchartcommon  ###############

set(kchartcommon_LIB_SRCS
   KChartFactory.cpp
   KChartPart.cpp
   KChartView.cpp
   KChartCanvas.cpp
   KChartViewAdaptor.cpp
   KChartPrintJob.cpp

   dialogs/KCPageLayout.cpp
   dialogs/KCPrinterDialog.cpp
)

kde4_add_kcfg_files(kchartcommon_LIB_SRCS prefs.kcfgc)

kde4_add_library(kchartcommon SHARED ${kchartcommon_LIB_SRCS})

target_link_libraries(kchartcommon
    kochart
    kdchart
    chartshapelib
    komain
    )

set_target_properties(kchartcommon
	PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION}
	SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION}
)

install(TARGETS kchartcommon  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### target kchartpart ###############

#set(kchartpart_PART_SRCS KChartFactoryInit.cpp )

#kde4_add_plugin(kchartpart WITH_PREFIX ${kchartpart_PART_SRCS})


#target_link_libraries(kchartpart
#	${KDE4_KDECORE_LIBS}
#	kchartcommon
	#chartshapelib
#)

#install(TARGETS kchartpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### target kchart ###############

#set(kchart_KDEINIT_SRCS main.cpp )

#kde4_add_kdeinit_executable( kchart ${kchart_KDEINIT_SRCS})

#target_link_libraries(kdeinit_kchart  ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} komain )

#install(TARGETS kdeinit_kchart  ${INSTALL_TARGETS_DEFAULT_ARGS})

#target_link_libraries( kchart kdeinit_kchart )
#install(TARGETS kchart  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  kchartpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
#install( FILES  kchart.rc kchart_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/kchart)
#install( PROGRAMS  kchart.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
#install( FILES  kchart.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
