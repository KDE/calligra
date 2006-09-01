include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon ${LCMS_INCLUDE_DIR} )

set(karbondockers_SRCS
    vcolordocker.cc
    vdocumentdocker.cc
    vstrokedocker.cc
    vstyledocker.cc
    vtransformdocker.cc
    )

kde4_automoc(${karbondockers_SRCS})

kde4_add_library(karbondockers ${karbondockers_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karbondockers )

#target_link_libraries( karbondockers ${KDE4_CORE} karbonpart )

install(TARGETS karbondockers  DESTINATION ${LIB_INSTALL_DIR})

