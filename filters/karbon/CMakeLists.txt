include_directories(
        ${KOTEXT_INCLUDES}
        ${KOMAIN_INCLUDES}
        ${KOPAINTER_INCLUDES}
	${FLAKE_INCLUDES}
        ${CMAKE_SOURCE_DIR}/karbon
        ${CMAKE_SOURCE_DIR}/karbon/core
        ${CMAKE_SOURCE_DIR}/karbon/visitors
        ${CMAKE_BINARY_DIR}/karbon
        ${KDE4_INCLUDES}  )

add_subdirectory( wpg )


# enable again when these filters are ported
if(FALSE)
add_subdirectory( eps )
add_subdirectory( ai )
add_subdirectory( svg )
add_subdirectory( png )
add_subdirectory( wmf )
add_subdirectory( kontour )
add_subdirectory( xcf )
add_subdirectory( msod )
add_subdirectory( oodraw )
endif(FALSE)




