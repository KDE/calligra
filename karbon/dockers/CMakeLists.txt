include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOGUIUTILS_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon ${LCMS_INCLUDE_DIR} )

set(karbondockers_SRCS
    vcolordocker.cc
    vdocumentdocker.cc
    vstyledocker.cc
    KarbonTransformDocker.cpp
    KarbonLayerDocker.cpp
    KarbonStylePreviewDocker.cpp
    )

kde4_add_library(karbondockers ${karbondockers_SRCS})



#target_link_libraries( karbondockers ${KDE4_CORE} karbonpart )

install(TARGETS karbondockers  DESTINATION ${LIB_INSTALL_DIR})

