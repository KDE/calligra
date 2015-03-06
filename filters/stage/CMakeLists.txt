
if(SHOULD_BUILD_FILTER_PPT_TO_ODP)
    add_subdirectory( powerpoint )
endif()

if(SHOULD_BUILD_FILTER_KPR_TO_ODP)
    add_subdirectory( kpr2odf )
endif()

if(SHOULD_BUILD_FILTER_PPTX_TO_ODP)
    add_subdirectory( pptx )
endif()

if(SHOULD_BUILD_FILTER_KEY_TO_ODP)
    add_subdirectory( keynote )
endif()
