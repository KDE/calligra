if(SHOULD_BUILD_FILTER_PPT_TO_ODP)
    add_subdirectory( powerpoint )
endif(SHOULD_BUILD_FILTER_PPT_TO_ODP)

if(SHOULD_BUILD_FILTER_KPR_TO_ODP)
    add_subdirectory( kpr2odf )
endif(SHOULD_BUILD_FILTER_KPR_TO_ODP)

if(SHOULD_BUILD_FILTER_PPTX_TO_ODP)
    add_subdirectory( pptx )
endif(SHOULD_BUILD_FILTER_PPTX_TO_ODP)
