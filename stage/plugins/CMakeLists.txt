if(Phonon4Qt5_FOUND)
    add_subdirectory( eventactions )
endif()
add_subdirectory( pageeffects )
add_subdirectory( shapeanimations )
if(NOT WIN32) ## Disable in Windows for now
	add_subdirectory( variable )
endif()
