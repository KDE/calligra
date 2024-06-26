add_definitions(-DTRANSLATION_DOMAIN=\"KarbonTools\")

include_directories(
    ${FLAKE_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/filterEffectTool
)

########### next target ###############

set(karbon_tools_PART_SRCS
   KarbonToolsPlugin.cpp
   KarbonCursor.cpp
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
    KarbonPatternOptionsWidget.cpp
)

ki18n_wrap_ui(karbon_tools_PART_SRCS
    filterEffectTool/FilterEffectEditWidget.ui
    KarbonPatternOptionsWidget.ui
)

add_library(karbon_tools MODULE ${karbon_tools_PART_SRCS})


target_link_libraries(karbon_tools kowidgets)

install(TARGETS karbon_tools DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/tools)

ecm_install_icons( ICONS
    22-actions-calligraphy.png
    22-actions-gradient.png
    22-actions-pattern.png
    32-actions-tool_imageeffects.png

    DESTINATION ${KDE_INSTALL_DATADIR}/calligra/icons
    THEME hicolor
)
