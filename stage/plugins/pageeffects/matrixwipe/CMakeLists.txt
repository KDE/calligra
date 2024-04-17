include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part ${CMAKE_SOURCE_DIR}/stage/part/pageeffects )

set(kpr_pageeffect_matrixwipe_SRCS
    Plugin.cpp
    KPrMatrixWipeStrategy.cpp

    snakewipe/KPrSnakeWipeEffectFactory.cpp
    snakewipe/KPrSnakeWipeBottomRightHorizontalStrategy.cpp
    snakewipe/KPrSnakeWipeBottomRightVerticalStrategy.cpp
    snakewipe/KPrSnakeWipeTopLeftHorizontalStrategy.cpp
    snakewipe/KPrSnakeWipeTopLeftVerticalStrategy.cpp
    snakewipe/KPrSnakeWipeDiagonalStrategy.cpp
    snakewipe/KPrSnakeWipeTopLeftDiagonalStrategy.cpp
    snakewipe/KPrSnakeWipeTopRightDiagonalStrategy.cpp
    snakewipe/KPrSnakeWipeBottomRightDiagonalStrategy.cpp
    snakewipe/KPrSnakeWipeBottomLeftDiagonalStrategy.cpp

    spiralwipe/KPrSpiralWipeEffectFactory.cpp
    spiralwipe/KPrSpiralWipeStrategy.cpp

    parallelsnakes/KPrParallelSnakesWipeEffectFactory.cpp
    parallelsnakes/KPrParallelSnakesWipeHorizontalStrategy.cpp
    parallelsnakes/KPrParallelSnakesWipeVerticalStrategy.cpp
    parallelsnakes/KPrParallelSnakesWipeDiagonalStrategy.cpp

    boxsnakes/KPrBoxSnakesWipeEffectFactory.cpp
    boxsnakes/KPrBoxSnakesWipeStrategy.cpp

    waterfallwipe/KPrWaterfallWipeEffectFactory.cpp
    waterfallwipe/KPrWaterfallWipeTopLeftStrategy.cpp
    waterfallwipe/KPrWaterfallWipeTopRightStrategy.cpp
    waterfallwipe/KPrWaterfallWipeBottomLeftStrategy.cpp
    waterfallwipe/KPrWaterfallWipeBottomRightStrategy.cpp
)

add_library(kpr_pageeffect_matrixwipe MODULE ${kpr_pageeffect_matrixwipe_SRCS})

target_link_libraries(
    kpr_pageeffect_matrixwipe
    calligrastageprivate 
    flake 

    Qt6::Core
    Qt6::Gui
)

install(TARGETS kpr_pageeffect_matrixwipe  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrastage/pageeffects)
