include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part ${CMAKE_SOURCE_DIR}/stage/part/pageeffects )

set(kpr_pageeffect_iriswipe_SRCS
    Plugin.cpp
    KPrIrisWipeEffectStrategyBase.cpp

    irisWipe/KPrIrisWipeEffectFactory.cpp
    triangleWipe/KPrTriangleWipeEffectFactory.cpp
    arrowHeadWipe/KPrArrowHeadWipeEffectFactory.cpp
    ellipseWipe/KPrEllipseWipeEffectFactory.cpp
    roundRectWipe/KPrRoundRectWipeEffectFactory.cpp
    pentagonWipe/KPrPentagonWipeEffectFactory.cpp
    hexagonWipe/KPrHexagonWipeEffectFactory.cpp
    starWipe/KPrStarWipeEffectFactory.cpp
    eyeWipe/KPrEyeWipeEffectFactory.cpp
    miscShapeWipe/KPrMiscShapeWipeEffectFactory.cpp
    )

add_library(kpr_pageeffect_iriswipe MODULE ${kpr_pageeffect_iriswipe_SRCS})

calligrastage_pageeffect_desktop_to_json(kpr_pageeffect_iriswipe kpr_pageeffect_iriswipe.desktop)

target_link_libraries(
    kpr_pageeffect_iriswipe
    calligrastageprivate
    flake
    
    
    Qt5::Core
    Qt5::Gui    
    )

install(TARGETS kpr_pageeffect_iriswipe  DESTINATION ${PLUGIN_INSTALL_DIR}/calligrastage/pageeffects)
