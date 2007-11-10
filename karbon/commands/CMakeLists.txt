include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karboncommands_SRCS
    KarbonLayerReorderCommand.cpp
    KarbonBooleanCommand.cpp
    )


kde4_add_library(karboncommands ${karboncommands_SRCS})

install(TARGETS karboncommands DESTINATION ${LIB_INSTALL_DIR})




