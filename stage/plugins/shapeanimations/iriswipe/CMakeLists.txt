include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part ${CMAKE_SOURCE_DIR}/stage/part/animations )

set(kpr_shapeeffect_iriswipe_SRCS
    Plugin.cpp
    KPrIrisWipeFilterEffectStrategyBase.cpp
    ellipseWipe/KPrEllipseWipeFilterEffectFactory.cpp
    )

kde4_add_plugin(kpr_shapeeffect_iriswipe ${kpr_shapeeffect_iriswipe_SRCS})

target_link_libraries(
    kpr_shapeeffect_iriswipe
    calligrastageprivate
    flake
    
    ${KDE4_KDECORE_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}    
    )

install(TARGETS kpr_shapeeffect_iriswipe  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kpr_shapeeffect_iriswipe.desktop DESTINATION ${SERVICES_INSTALL_DIR})


