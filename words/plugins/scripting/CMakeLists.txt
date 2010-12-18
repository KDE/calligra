include_directories( 
	${CMAKE_SOURCE_DIR}/kword/part
	${CMAKE_SOURCE_DIR}/kword/part/frames
	${KOTEXT_INCLUDES}
	${KOKROSS_INCLUDES} )

add_subdirectory( scripts )

set(krossmodulekword_PART_SRCS
	Module.cpp
	TextCursor.cpp
	KWScriptingPart.cpp
)

qt4_wrap_cpp(krossmodulekword_PART_SRCS
	Page.h PageLayout.h
	Style.h
	TextDocument.h TextFrame.h TextList.h TextTable.h
	Frame.h FrameSet.h
	Tool.h
)

kde4_add_plugin(krossmodulekword ${krossmodulekword_PART_SRCS})
target_link_libraries(krossmodulekword kokross kwordprivate)
install(TARGETS krossmodulekword DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES krossmodulekword.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES scripting.rc DESTINATION ${DATA_INSTALL_DIR}/kword/kpartplugins)
