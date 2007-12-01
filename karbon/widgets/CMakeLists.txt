include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOGUIUTILS_INCLUDES} ${FLAKE_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karbonwidgets_SRCS
    KarbonCanvas.cpp
    vcolorslider.cc
    vgradientwidget.cc
    KarbonGradientItem.cpp
    vgradienttabwidget.cc
    KarbonStylePreview.cpp
#vsmallpreview.cc
    KarbonStyleButtonBox.cpp
    )


kde4_add_library(karbonwidgets ${karbonwidgets_SRCS})

target_link_libraries(karbonwidgets flake)

install(TARGETS karbonwidgets  DESTINATION ${LIB_INSTALL_DIR})

