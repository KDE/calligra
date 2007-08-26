
project(kchart)

add_subdirectory( kdchart )
add_subdirectory( shape )
add_subdirectory( toolbar )
add_subdirectory( pics )
add_subdirectory( templates )

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/kdchart/include
	${CMAKE_CURRENT_SOURCE_DIR}/kdchart/src
	${CMAKE_CURRENT_SOURCE_DIR}/kdchart/examples/tools
	${CMAKE_CURRENT_SOURCE_DIR}/dialogs
	${CMAKE_SOURCE_DIR}/interfaces 
	${KOGUIUTILS_INCLUDES} )


########### next target ###############

set(kchartcommon_LIB_SRCS 
   kchart_factory.cpp
   kchart_part.cpp
   kchart_view.cpp
   KChartViewAdaptor.cpp

   commands/ChartTypeCommand.cpp

   dialogs/KCPageLayout.cpp
   dialogs/KCPrinterDialog.cpp

   kdchart/examples/tools/TableModel.cpp
)


kde4_add_library(kchartcommon SHARED ${kchartcommon_LIB_SRCS})

target_link_libraries(kchartcommon  ${KDE4_KDE3SUPPORT_LIBS} kochart koguiutils kdchart )

set_target_properties(kchartcommon PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kchartcommon  DESTINATION ${LIB_INSTALL_DIR})


########### next target ###############

set(kchartpart_PART_SRCS kchart_factory_init.cpp )


kde4_add_plugin(kchartpart WITH_PREFIX ${kchartpart_PART_SRCS})



target_link_libraries(kchartpart  ${KDE4_KDECORE_LIBS} kchartcommon )

install(TARGETS kchartpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kchart_KDEINIT_SRCS main.cpp )

kde4_add_kdeinit_executable( kchart ${kchart_KDEINIT_SRCS})

target_link_libraries(kdeinit_kchart  ${KDE4_KDECORE_LIBS} komain )

install(TARGETS kdeinit_kchart  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kchart kdeinit_kchart )
install(TARGETS kchart  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  kchartpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kchart.rc kchart_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/kchart)
install( FILES  kchart.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})


