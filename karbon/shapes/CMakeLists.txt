include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karbonshapes_SRCS
    vellipse.cc 
    vpolyline.cc 
    vpolygon.cc 
    #vrectangle.cc 
    vsinus.cc 
    vspiral.cc 
    vstar.cc
    )

kde4_automoc(${karbonshapes_SRCS})

kde4_add_library(karbonshapes ${karbonshapes_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karbonshapes )

install(TARGETS karbonshapes  DESTINATION ${LIB_INSTALL_DIR})





########### next target ###############


########### install files ###############

