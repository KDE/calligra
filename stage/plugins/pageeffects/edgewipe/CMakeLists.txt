include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part ${CMAKE_SOURCE_DIR}/stage/part/pageeffects )

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

add_library(kpr_pageeffect_edgewipe MODULE ${kpr_pageeffect_edgewipe_SRCS})

target_link_libraries(
    kpr_pageeffect_edgewipe
    calligrastageprivate
    flake
    
    
    Qt6::Core
    Qt6::Gui    
    )

install(TARGETS kpr_pageeffect_edgewipe  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrastage/pageeffects)
