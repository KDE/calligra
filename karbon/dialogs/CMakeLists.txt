include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karbondialogs_SRCS
#    vcolordlg.cc 
    KarbonConfigureDialog.cpp
#    vcolortab.cc 
    )

kde4_add_library(karbondialogs ${karbondialogs_SRCS})



install(TARGETS karbondialogs  DESTINATION ${LIB_INSTALL_DIR})

