

add_subdirectory( kabc )
add_subdirectory( kspread )

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir sql need test it")
include_directories( ${CMAKE_SOURCE_DIR}/lib/kformula ${CMAKE_SOURCE_DIR}/kword ${KOTEXT_INCLUDES} ${KOFFICEUI_INCLUDES} )


########### next target ###############

set(kwmailmerge_classic_PART_SRCS KWClassicSerialDataSource.cpp )

kde4_automoc(${kwmailmerge_classic_PART_SRCS})

kde4_add_plugin(kwmailmerge_classic ${kwmailmerge_classic_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwmailmerge_classic )

target_link_libraries(kwmailmerge_classic  kwmailmerge_interface ${KDE4_KDEUI_LIBS} kotext )

install(TARGETS kwmailmerge_classic  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kwserialletter_classic.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

#
