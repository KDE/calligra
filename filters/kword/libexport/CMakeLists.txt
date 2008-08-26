

include_directories( ${KOMAIN_INCLUDES} )


########### next target ###############

set(kwordexportfilters_LIB_SRCS 
   TagProcessing.cc 
   KWEFStructures.cc 
   ProcessDocument.cc 
   KWEFUtil.cc 
   KWEFKWordLeader.cc 
   KWEFBaseWorker.cc )


kde4_add_library(kwordexportfilters SHARED ${kwordexportfilters_LIB_SRCS})

target_link_libraries(
    kwordexportfilters
    komain
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTXML_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY} 
    )

set_target_properties(kwordexportfilters PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kwordexportfilters  DESTINATION ${LIB_INSTALL_DIR})

