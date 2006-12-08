include_directories( 
	${CMAKE_SOURCE_DIR}/kword/part
	${KOTEXT_INCLUDES} 
	${KOFFICEUI_INCLUDES} 
	${KROSS_INCLUDES} )

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

# the KWord Scripting KParts::Plugin + Kross module
#set(krossmodulekword_PART_SRCS ScriptingModule.cpp ScriptingFunction.cpp ScriptingPart.cpp)
set(krossmodulekword_PART_SRCS ScriptingModule.cpp ScriptingPart.cpp)
kde4_automoc(${krossmodulekword_PART_SRCS})
kde4_add_plugin(krossmodulekword ${krossmodulekword_PART_SRCS})

#target_link_libraries(krossmodulekword  ${KDE4_KDECORE_LIBS} kspreadcommon ${KDE4_KROSSCORE_LIBS} )
target_link_libraries(krossmodulekword  ${KDE4_KDECORE_LIBS} ${KDE4_KROSSCORE_LIBS} kparts)
install(TARGETS krossmodulekword DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  krossmodulekword.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  scripting.rc  DESTINATION ${DATA_INSTALL_DIR}/kword/kpartplugins)
