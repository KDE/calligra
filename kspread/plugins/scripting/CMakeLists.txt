include_directories( 
	${CMAKE_SOURCE_DIR}/kspread 
	${KOTEXT_INCLUDES} 
	${KOFFICEUI_INCLUDES} 
	${KROSS_INCLUDES} )

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

#disabled for now till ported to kross2
##add_subdirectory( kspreadcore )

add_subdirectory( scripts )

########### next target ###############

set(kspreadscripting_PART_SRCS scripting.cpp )

kde4_automoc(${kspreadscripting_PART_SRCS})

kde4_add_plugin(kspreadscripting ${kspreadscripting_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kspreadscripting )

target_link_libraries(kspreadscripting  ${KDE4_KDECORE_LIBS} kspreadcommon krosscore )

install(TARGETS kspreadscripting DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kspreadscripting.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  scripting.rc  DESTINATION ${DATA_INSTALL_DIR}/kspread/kpartplugins)
