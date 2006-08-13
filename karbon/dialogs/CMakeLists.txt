include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karbondialogs_SRCS
    vcolordlg.cc 
    vconfiguredlg.cc 
    vcolortab.cc 
    )

kde4_automoc(${karbondialogs_SRCS})

kde4_add_library(karbondialogs ${karbondialogs_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karbondialogs )

install(TARGETS karbondialogs  DESTINATION ${LIB_INSTALL_DIR})

