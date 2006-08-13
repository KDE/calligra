include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karbonvisitors_SRCS
    vselectnodes.cc 
    vselectobjects.cc 
    vdrawselection.cc 
    vselectiondesc.cc 
    vtransformnodes.cc 
    )

kde4_automoc(${karbonvisitors_SRCS})

kde4_add_library(karbonvisitors ${karbonvisitors_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karbonvisitors )

install(TARGETS karbonvisitors  DESTINATION ${LIB_INSTALL_DIR})


