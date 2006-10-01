include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karboncommands_SRCS
    vbooleancmd.cc 
    vcleanupcmd.cc 
    vclipartcmd.cc 
    vclosepathcmd.cc 
    vcommand.cc 
    vfillcmd.cc 
    vflattencmd.cc 
    vlayercmd.cc
    vreplacingcmd.cc 
    vshapecmd.cc 
    vstrokecmd.cc 
    vtransformcmd.cc 
    vinsertcmd.cc 
    vzordercmd.cc
    )

kde4_automoc(${karboncommands_SRCS})

kde4_add_library(karboncommands ${karboncommands_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karboncommands )

install(TARGETS karboncommands  DESTINATION ${LIB_INSTALL_DIR})




