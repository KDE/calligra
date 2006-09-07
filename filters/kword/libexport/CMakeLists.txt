

include_directories( ${KOFFICECORE_INCLUDES} )


########### next target ###############

set(kwordexportfilters_LIB_SRCS 
   TagProcessing.cc 
   KWEFStructures.cc 
   ProcessDocument.cc 
   KWEFUtil.cc 
   KWEFKWordLeader.cc 
   KWEFBaseWorker.cc )

kde4_automoc(${kwordexportfilters_LIB_SRCS})

kde4_add_library(kwordexportfilters SHARED ${kwordexportfilters_LIB_SRCS})

target_link_libraries(kwordexportfilters  ${KDE4_KDECORE_LIBS} kofficecore )

set_target_properties(kwordexportfilters PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kwordexportfilters  DESTINATION ${LIB_INSTALL_DIR})

