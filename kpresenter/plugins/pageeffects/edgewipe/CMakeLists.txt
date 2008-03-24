include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${KOGUIUTILS_INCLUDES} ${CMAKE_SOURCE_DIR}/kpresenter/part ${CMAKE_SOURCE_DIR}/kpresenter/part/pageeffects )

set(kpr_pageeffect_edgewipe_SRCS
    Plugin.cpp

    miscdiagonalwipe/MiscDiagonalWipeEffectFactory.cpp
    miscdiagonalwipe/DoubleBarnDoorWipeStrategy.cpp
    miscdiagonalwipe/DoubleDiamondWipeStrategy.cpp

    fourboxwipe/FourBoxWipeEffectFactory.cpp
    fourboxwipe/CornersInWipeStrategy.cpp
    fourboxwipe/CornersOutWipeStrategy.cpp

    diagonalwipe/DiagonalWipeEffectFactory.cpp
    diagonalwipe/DiagonalWipeStrategy.cpp

    barndoorwipe/BarnDoorWipeEffectFactory.cpp
    barndoorwipe/BarnDoorWipeStrategy.cpp

    boxwipe/BoxWipeEffectFactory.cpp
    boxwipe/BoxWipeStrategy.cpp

    veewipe/VeeWipeEffectFactory.cpp
    veewipe/VeeWipeStrategy.cpp

    barnveewipe/BarnVeeWipeEffectFactory.cpp
    barnveewipe/BarnVeeWipeStrategy.cpp

    zigzagwipe/ZigZagWipeEffectFactory.cpp
    zigzagwipe/ZigZagWipeStrategy.cpp

    barnzigzagwipe/BarnZigZagWipeEffectFactory.cpp
    barnzigzagwipe/BarnZigZagWipeStrategy.cpp
    )

kde4_add_plugin(kpr_pageeffect_edgewipe ${kpr_pageeffect_edgewipe_SRCS})

target_link_libraries(kpr_pageeffect_edgewipe kpresenterprivate flake koguiutils)

install(TARGETS kpr_pageeffect_edgewipe  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kpr_pageeffect_edgewipe.desktop DESTINATION ${SERVICES_INSTALL_DIR})


