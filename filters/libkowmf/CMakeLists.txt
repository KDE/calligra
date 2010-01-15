

include_directories( ${KOMAIN_INCLUDES} )


########### next target ###############

set(kwmf_LIB_SRCS kwmf.cc qwmf.cc )


kde4_add_library(kwmf SHARED ${kwmf_LIB_SRCS})

target_link_libraries(kwmf  ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

set_target_properties(kwmf PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kwmf  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(kowmf_LIB_SRCS 
   kowmfreadprivate.cc 
   kowmfstack.cc 
   kowmfread.cc 
   kowmfwrite.cc 
   kowmfpaint.cc )

kde4_add_library(kowmf SHARED ${kowmf_LIB_SRCS})

target_link_libraries(kowmf  ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

set_target_properties(kowmf PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kowmf  ${INSTALL_TARGETS_DEFAULT_ARGS})

