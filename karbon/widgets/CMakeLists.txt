include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOPAINTER_INCLUDES} ${FLAKE_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karbonwidgets_SRCS
    vcanvas.cc
    vcolorslider.cc
    vgradientwidget.cc
    vgradienttabwidget.cc
    vstrokefillpreview.cc
    vselecttoolbar.cc
    vsmallpreview.cc
    vtypebuttonbox.cc
    KarbonLineStyleSelector.cpp
    )

kde4_automoc(${karbonwidgets_SRCS})

kde4_add_library(karbonwidgets ${karbonwidgets_SRCS})

target_link_libraries(karbonwidgets flake)

install(TARGETS karbonwidgets  DESTINATION ${LIB_INSTALL_DIR})

