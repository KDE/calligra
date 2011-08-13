include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part ${CMAKE_SOURCE_DIR}/stage/part/pageeffects )

set(kpr_pageeffect_slidewipe_PART_SRCS Plugin.cpp
KPrSlideWipeEffectFactory.cpp
KPrSlideWipeFromBottomStrategy.cpp
KPrSlideWipeFromLeftStrategy.cpp
KPrSlideWipeFromRightStrategy.cpp
KPrSlideWipeFromTopStrategy.cpp
KPrSlideWipeToBottomStrategy.cpp
KPrSlideWipeToLeftStrategy.cpp
KPrSlideWipeToRightStrategy.cpp
KPrSlideWipeToTopStrategy.cpp
)


kde4_add_plugin(kpr_pageeffect_slidewipe ${kpr_pageeffect_slidewipe_PART_SRCS})

target_link_libraries(
    kpr_pageeffect_slidewipe
    calligrastageprivate
    flake
    
    ${KDE4_KDECORE_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}    
    )

install(TARGETS kpr_pageeffect_slidewipe  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kpr_pageeffect_slidewipe.desktop DESTINATION ${SERVICES_INSTALL_DIR})


