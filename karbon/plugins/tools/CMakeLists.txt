include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

########### next target ###############

set(karbon_tools_PART_SRCS
   KarbonToolsPlugin.cpp
   KarbonCurveFit.cpp
   KarbonPencilTool.cpp
   KarbonPencilToolFactory.cpp
   KarbonGradientTool.cpp
   KarbonGradientToolFactory.cpp
   KarbonPatternTool.cpp
   KarbonPatternToolFactory.cpp
)

kde4_automoc(${karbon_tools_PART_SRCS})

kde4_add_plugin(karbontools ${karbon_tools_PART_SRCS})

target_link_libraries(karbontools ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kofficeui kopainter karboncommon )

install(TARGETS karbontools DESTINATION ${PLUGIN_INSTALL_DIR})

kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons  )

########### install files ###############

install( FILES karbontools.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
