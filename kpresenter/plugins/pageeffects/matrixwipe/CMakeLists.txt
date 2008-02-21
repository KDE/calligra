include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${KOGUIUTILS_INCLUDES} ${CMAKE_SOURCE_DIR}/kpresenter/part ${CMAKE_SOURCE_DIR}/kpresenter/part/pageeffects )

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
    )

kde4_add_plugin(kpr_pageeffect_matrixwipe ${kpr_pageeffect_matrixwipe_SRCS})

target_link_libraries(kpr_pageeffect_matrixwipe kpresenterprivate flake koguiutils)

install(TARGETS kpr_pageeffect_matrixwipe  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kpr_pageeffect_matrixwipe.desktop DESTINATION ${SERVICES_INSTALL_DIR})


