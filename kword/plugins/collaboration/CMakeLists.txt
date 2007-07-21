include_directories( 
	${CMAKE_SOURCE_DIR}/kword/part
	${CMAKE_SOURCE_DIR}/kword/part/frames
	${KOTEXT_INCLUDES} 
	${KOGUIUTILS_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR} )

add_subdirectory( libcollaboration )
add_subdirectory( undoframework_testapp )

set(collaborationmodulekword_PART_SRCS
	KWCollaborationPart.cpp
)

kde4_add_plugin(collaborationmodulekword ${collaborationmodulekword_PART_SRCS})
target_link_libraries(collaborationmodulekword ${KDE4_KPARTS_LIBS} kwordprivate collaboration)
install(TARGETS collaborationmodulekword DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES collaborationmodulekword.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES collaboration.rc DESTINATION ${DATA_INSTALL_DIR}/kword/kpartplugins)
