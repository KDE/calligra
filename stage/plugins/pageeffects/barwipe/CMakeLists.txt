include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part ${CMAKE_SOURCE_DIR}/stage/part/pageeffects )

set(kpr_pageeffect_barwipe_PART_SRCS Plugin.cpp
    KPrBarWipeEffectFactory.cpp
    KPrBarWipeFromLeftStrategy.cpp
    KPrBarWipeFromRightStrategy.cpp
    KPrBarWipeFromTopStrategy.cpp
    KPrBarWipeFromBottomStrategy.cpp
)

add_library(kpr_pageeffect_barwipe MODULE ${kpr_pageeffect_barwipe_PART_SRCS})

target_link_libraries(
    kpr_pageeffect_barwipe
    calligrastageprivate
    flake
    Qt6::Core
    Qt6::Gui
)

install(TARGETS kpr_pageeffect_barwipe  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrastage/pageeffects)
