include_directories(${KOMAIN_INCLUDES}  ${KOTEXT_INCLUDES})

add_subdirectory( powerpoint )
add_subdirectory( kpr2odf )
if(SHOULD_BUILD_OOXML_FILTERS)
    add_subdirectory( pptx )
endif(SHOULD_BUILD_OOXML_FILTERS)
