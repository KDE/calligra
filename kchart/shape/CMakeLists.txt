include_directories( ${CMAKE_SOURCE_DIR}/kchart
                     ${CMAKE_SOURCE_DIR}/kchart/kdchart/src
                     ${CMAKE_SOURCE_DIR}/interfaces
                     ${KOGUIUTILS_INCLUDES} )

########### Flake Plugin library ###############

SET (chartshape_LIB_SRCS
	ChartShape.cpp
	ChartShapeFactory.cpp
        ChartTypeTool.cpp
        ChartTypeToolFactory.cpp
	ChartTypeConfigWidget.cpp
	../commands/ChartTypeCommand.cpp
	)

kde4_add_library(chartshapelib SHARED ${chartshape_LIB_SRCS})
target_link_libraries(chartshapelib kdchart kochart flake)
set_target_properties(chartshapelib PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS chartshapelib DESTINATION ${LIB_INSTALL_DIR})

kde4_add_plugin(chartshape ${chartshape_LIB_SRCS})
target_link_libraries(chartshape kdchart kochart flake)
install(TARGETS chartshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES chartshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
