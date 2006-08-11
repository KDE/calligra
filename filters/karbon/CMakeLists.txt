include_directories(
        ${KOTEXT_INCLUDES}
        ${KOFFICECORE_INCLUDES}
        ${KOPAINTER_INCLUDES}
	${FLAKE_INCLUDES}
        ${CMAKE_SOURCE_DIR}/karbon
        ${CMAKE_SOURCE_DIR}/karbon/core
        ${CMAKE_SOURCE_DIR}/karbon/visitors
        ${CMAKE_BINARY_DIR}/karbon
        ${KDE4_INCLUDES}  )

add_subdirectory( eps )
add_subdirectory( ai )
add_subdirectory( svg )
add_subdirectory( png )
add_subdirectory( wmf )
add_subdirectory( kontour )
add_subdirectory( xcf )
add_subdirectory( msod )
add_subdirectory( oodraw )




########### install files ###############






#original Makefile.am contents follow:

#SUBDIRS = eps ai svg png wmf kontour xcf msod oodraw
#
