

include_directories( 
	${CMAKE_SOURCE_DIR}/kspread 
	${CMAKE_SOURCE_DIR}/kword 
	${KOTEXT_INCLUDES} 
	${KOFFICEUI_INCLUDES} )


########### next target ###############

set(kwmailmerge_kspread_PART_SRCS kwmailmerge_kspread.cpp kwmailmerge_kspread_config.cpp )

kde4_automoc(${kwmailmerge_kspread_PART_SRCS})

kde4_add_plugin(kwmailmerge_kspread ${kwmailmerge_kspread_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwmailmerge_kspread )

target_link_libraries(kwmailmerge_kspread  kwmailmerge_interface kspreadcommon ${KDE4_KDEUI_LIBS} )

install(TARGETS kwmailmerge_kspread  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kwmailmerge_kspread.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


