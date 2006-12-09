include_directories( 
	${CMAKE_SOURCE_DIR}/kword/part
	${KOTEXT_INCLUDES} 
	${KOFFICEUI_INCLUDES} 
	${KROSS_INCLUDES} )

set(krossmodulekword_PART_SRCS KWScriptingModule.cpp KWScriptingPart.cpp)
kde4_automoc(${krossmodulekword_PART_SRCS})
kde4_add_plugin(krossmodulekword ${krossmodulekword_PART_SRCS})
target_link_libraries(krossmodulekword  ${KDE4_KDECORE_LIBS} ${KDE4_KROSSCORE_LIBS} kparts kwordprivate)
install(TARGETS krossmodulekword DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  krossmodulekword.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  scripting.rc  DESTINATION ${DATA_INSTALL_DIR}/kword/kpartplugins)
