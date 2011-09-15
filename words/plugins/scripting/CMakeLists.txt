include_directories( 
	${CMAKE_SOURCE_DIR}/words/part
	${CMAKE_SOURCE_DIR}/words/part/frames
	${KOTEXT_INCLUDES}
	${KOKROSS_INCLUDES} )

add_subdirectory( scripts )

set(krossmodulewords_PART_SRCS
	Module.cpp
	TextCursor.cpp
	KWScriptingPart.cpp
)

qt4_wrap_cpp(krossmodulewords_PART_SRCS
	Page.h PageLayout.h
	Style.h
	TextDocument.h TextFrame.h TextList.h TextTable.h
	Frame.h FrameSet.h
	Tool.h
)

kde4_add_plugin(krossmodulewords ${krossmodulewords_PART_SRCS})
target_link_libraries(krossmodulewords kokross wordsprivate)
install(TARGETS krossmodulewords DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES krossmodulewords.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES scripting.rc DESTINATION ${DATA_INSTALL_DIR}/words/kpartplugins)
