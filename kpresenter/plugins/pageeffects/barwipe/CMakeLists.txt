include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${KOGUIUTILS_INCLUDES} ${CMAKE_SOURCE_DIR}/kpresenter/part ${CMAKE_SOURCE_DIR}/kpresenter/part/pageeffects )

set(kpr_pageeffect_barwipe_PART_SRCS Plugin.cpp
    KPrBarWipeEffectFactory.cpp
    KPrBarWipeFromLeftStrategy.cpp
    KPrBarWipeFromTopStrategy.cpp
)

kde4_add_plugin(kpr_pageeffect_barwipe ${kpr_pageeffect_barwipe_PART_SRCS})

target_link_libraries(
    kpr_pageeffect_barwipe
    kpresenterprivate
    flake 
    koguiutils
    ${KDE4_KDECORE_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )

install(TARGETS kpr_pageeffect_barwipe  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kpr_pageeffect_barwipe.desktop DESTINATION ${SERVICES_INSTALL_DIR})


