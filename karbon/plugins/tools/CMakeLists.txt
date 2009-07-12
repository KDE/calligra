include_directories(
    ${QT_INCLUDES} 
    ${KDE4_INCLUDE_DIR}
    ${FLAKE_INCLUDES}
    ${CMAKE_SOURCE_DIR}/karbon 
    ${CMAKE_SOURCE_DIR}/karbon/ui
    ${CMAKE_SOURCE_DIR}/karbon/ui/widgets 
)

########### next target ###############

set(karbon_tools_PART_SRCS
   KarbonToolsPlugin.cpp
   KarbonCurveFit.cpp
   KarbonPencilTool.cpp
   KarbonPencilToolFactory.cpp
   CalligraphyTool/KarbonCalligraphyTool.cpp
   CalligraphyTool/KarbonCalligraphyOptionWidget.cpp
   CalligraphyTool/KarbonCalligraphyToolFactory.cpp
   CalligraphyTool/KarbonCalligraphicShape.cpp
   CalligraphyTool/KarbonCalligraphicShapeFactory.cpp
   CalligraphyTool/KarbonSimplifyPath.cpp
   KarbonGradientTool.cpp
   KarbonGradientToolFactory.cpp
   KarbonGradientEditStrategy.cpp
   KarbonPatternTool.cpp
   KarbonPatternToolFactory.cpp
   KarbonPatternEditStrategy.cpp
   KarbonFilterEffectsTool.cpp
   KarbonFilterEffectsToolFactory.cpp
)


kde4_add_plugin(karbontools ${karbon_tools_PART_SRCS})

target_link_libraries(karbontools koresources pigmentcms flake koresourcewidget ${KDE4_KPARTS_LIBS}  karboncommon karbonui )

install(TARGETS karbontools DESTINATION ${PLUGIN_INSTALL_DIR})

kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons  )

########### install files ###############

install( FILES karbontools.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
