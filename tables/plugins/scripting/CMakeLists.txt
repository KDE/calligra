include_directories( 
	${CMAKE_SOURCE_DIR}/kspread
    ${CMAKE_SOURCE_DIR}/kspread/commands
	${CMAKE_SOURCE_DIR}/kspread/ui
	${KOTEXT_INCLUDES} 
	${KOMAIN_INCLUDES} 
	${KOKROSS_INCLUDES}
	)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

add_subdirectory( scripts )

# the KSpread Scripting KParts::Plugin + Kross module
set(krossmodulekspread_PART_SRCS ScriptingModule.cpp ScriptingFunction.cpp ScriptingWidgets.cpp ScriptingPart.cpp ScriptingListener.cpp)

qt4_wrap_cpp(krossmodulekspread_PART_SRCS ScriptingReader.h ScriptingWriter.h)
kde4_add_plugin(krossmodulekspread ${krossmodulekspread_PART_SRCS})

target_link_libraries(krossmodulekspread ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} kspreadcommon kokross)
install(TARGETS krossmodulekspread DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

#install( FILES  kspreadscripting.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  krossmodulekspread.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  scripting.rc  DESTINATION ${DATA_INSTALL_DIR}/kspread/kpartplugins)
