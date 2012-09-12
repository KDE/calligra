

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDES}  )


########### next target ###############

set(kspreadexport_LIB_SRCS KSpreadLeader.cc KSpreadBaseWorker.cc )


kde4_add_library(kspreadexport SHARED ${kspreadexport_LIB_SRCS})

target_link_libraries(kspreadexport  ${KDE4_KDECORE_LIBS} )

set_target_properties(kspreadexport PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS kspreadexport  ${INSTALL_TARGETS_DEFAULT_ARGS})
