include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(vpainter_SRCS
    vpainterfactory.cc
    vqpainter.cc
    )

kde4_automoc(${vpainter_SRCS})

kde4_add_library(vpainter ${vpainter_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} vpainter )

install(TARGETS vpainter  DESTINATION ${LIB_INSTALL_DIR})



