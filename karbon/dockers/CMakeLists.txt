include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOGUIUTILS_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon ${LCMS_INCLUDE_DIR} )

set(karbondockers_SRCS
    vcolordocker.cc
#    vdocumentdocker.cc
#vstyledocker.cc
    KarbonTransformDocker.cpp
    KarbonLayerDocker.cpp
    KarbonLayerModel.cpp
    KarbonLayerSortingModel.cpp
    KarbonStylePreviewDocker.cpp
    )

kde4_add_library(karbondockers ${karbondockers_SRCS})

install(TARGETS karbondockers  DESTINATION ${LIB_INSTALL_DIR})

