include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part ${CMAKE_SOURCE_DIR}/stage/part/pageeffects )

set(kpr_pageeffect_spacerotation_PART_SRCS Plugin.cpp
    KPrSpaceRotationEffectFactory.cpp
    KPrSpaceRotationFromBottomStrategy.cpp
    KPrSpaceRotationFromTopStrategy.cpp
    KPrSpaceRotationFromRightStrategy.cpp
    KPrSpaceRotationFromLeftStrategy.cpp
)

add_library(kpr_pageeffect_spacerotation MODULE ${kpr_pageeffect_spacerotation_PART_SRCS})

target_link_libraries(
    kpr_pageeffect_spacerotation
    calligrastageprivate
    flake

    Qt6::Core
    Qt6::Gui
)

install(TARGETS kpr_pageeffect_spacerotation  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrastage/pageeffects)
