project(kformula)

set(KFORMULA_INCLUDES
    ${CMAKE_SOURCE_DIR}/kformula
    ${CMAKE_SOURCE_DIR}/kformula/flake
    ${CMAKE_SOURCE_DIR}/kformula/flake/elements
    ${KOMAIN_INCLUDES}     
    )

include_directories(${KFORMULA_INCLUDES})

add_subdirectory( pics )
add_subdirectory( flake )

########### next target ###############

set(kformulaprivate_LIB_SRCS
   KFormulaPartDocument.cpp 
   KFormulaPartView.cpp 
   KFormulaPartViewAdaptor.cpp
   KFormulaPartFactory.cpp 
   KFormulaCanvas.cpp 
   KFormulaConfigDialog.cpp )
	
kde4_add_library(kformulaprivate SHARED ${kformulaprivate_LIB_SRCS})

target_link_libraries(kformulaprivate kformulalib)

set_target_properties(kformulaprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )


install(TARGETS kformulaprivate ${INSTALL_TARGETS_DEFAULT_ARGS})

########### next target ###############

set(kformulapart_PART_SRCS KFormulaPartFactoryInit.cpp )


kde4_add_plugin(kformulapart WITH_PREFIX ${kformulapart_PART_SRCS})

target_link_libraries(kformulapart ${KDE4_KPARTS_LIBS} kformulaprivate)

install(TARGETS kformulapart  DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############

set(kformula_KDEINIT_SRCS main.cpp )

kde4_add_app_icon(kformula_KDEINIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-app-kformula.png")

kde4_add_kdeinit_executable( kformula_bin ${kformula_KDEINIT_SRCS})

target_link_libraries(kdeinit_kformula_bin komain )

install(TARGETS kdeinit_kformula_bin  ${INSTALL_TARGETS_DEFAULT_ARGS})

set_target_properties(kdeinit_kformula_bin PROPERTIES OUTPUT_NAME kdeinit_kformula)

target_link_libraries( kformula_bin kdeinit_kformula_bin )
set_target_properties( kformula_bin PROPERTIES OUTPUT_NAME kformula)

install(TARGETS kformula_bin  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  tips  DESTINATION ${DATA_INSTALL_DIR}/kformula)
install( FILES  kformulapart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kformula.rc kformula_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/kformula)
install( FILES  kformula.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})


