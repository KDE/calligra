include_directories(
     
    
    ${FLAKE_INCLUDES}
    ${CMAKE_SOURCE_DIR}/karbon 
    ${CMAKE_SOURCE_DIR}/karbon/ui
    ${CMAKE_SOURCE_DIR}/karbon/ui/widgets 
    ${CMAKE_CURRENT_SOURCE_DIR}/filterEffectTool
)

########### next target ###############

set(karbon_tools_PART_SRCS
   KarbonToolsPlugin.cpp
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
   filterEffectTool/KarbonFilterEffectsTool.cpp
   filterEffectTool/KarbonFilterEffectsToolFactory.cpp
   filterEffectTool/FilterEffectEditWidget.cpp
   filterEffectTool/FilterEffectScene.cpp
   filterEffectTool/FilterEffectSceneItems.cpp
   filterEffectTool/FilterInputChangeCommand.cpp
   filterEffectTool/FilterAddCommand.cpp
   filterEffectTool/FilterRemoveCommand.cpp
   filterEffectTool/FilterStackSetCommand.cpp
   filterEffectTool/FilterRegionChangeCommand.cpp
   filterEffectTool/FilterEffectResource.cpp
   filterEffectTool/FilterResourceServerProvider.cpp
   filterEffectTool/FilterRegionEditStrategy.cpp
)

ki18n_wrap_ui(karbon_tools_PART_SRCS
    filterEffectTool/FilterEffectEditWidget.ui
)

add_library(karbontools MODULE ${karbon_tools_PART_SRCS})

target_link_libraries(karbontools karboncommon karbonui kowidgets)

install(TARGETS karbontools DESTINATION ${PLUGIN_INSTALL_DIR})

ecm_install_icons( ${DATA_INSTALL_DIR}/calligra/icons  )

########### install files ###############

install( FILES karbontools.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
