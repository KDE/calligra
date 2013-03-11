add_subdirectory( eventactions )
add_subdirectory( pageeffects )
add_subdirectory( shapeanimations )
if(NOT WIN32) ## Disable in Windows for now
	add_subdirectory( variable )
endif(NOT WIN32)
