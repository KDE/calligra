include_directories( 
	${CMAKE_SOURCE_DIR}/kspread 
	${KOTEXT_INCLUDES} 
	${KOFFICEUI_INCLUDES} 
	${KROSS_INCLUDES} )

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

add_subdirectory( scripts )

# the KSpread Scripting KParts::Plugin + Kross module
set(krossmodulekspread_PART_SRCS ScriptingModule.cpp ScriptingFunction.cpp ScriptingPart.cpp)
kde4_automoc(${krossmodulekspread_PART_SRCS})
kde4_add_plugin(krossmodulekspread ${krossmodulekspread_PART_SRCS})

target_link_libraries(krossmodulekspread  ${KDE4_KDECORE_LIBS} kspreadcommon ${KDE4_KROSSCORE_LIBS} )
install(TARGETS krossmodulekspread DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

#install( FILES  kspreadscripting.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  krossmodulekspread.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  scripting.rc  DESTINATION ${DATA_INSTALL_DIR}/kspread/kpartplugins)
