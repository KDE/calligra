

include_directories(
    ${KOMAIN_INCLUDES}
	)

add_subdirectory( genericimageexport )
if (QT3SUPPORT)
    add_subdirectory( svg )
endif (QT3SUPPORT)
