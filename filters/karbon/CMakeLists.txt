
include_directories(
        ${KOTEXT_INCLUDES}
        ${KOMAIN_INCLUDES}
        ${FLAKE_INCLUDES}
        ${KOPAGEAPP_INCLUDES}
        ${CMAKE_SOURCE_DIR}/karbon
        ${CMAKE_SOURCE_DIR}/karbon/common
        ${CMAKE_SOURCE_DIR}/karbon/ui
        ${CMAKE_BINARY_DIR}/karbon
        
        ${CMAKE_SOURCE_DIR}/plugins/ )

if(SHOULD_BUILD_FILTER_KARBON_TO_IMAGE)
    add_subdirectory( image )
endif()

add_subdirectory( svg )

if(SHOULD_BUILD_FILTER_WPG_TO_SVG)
  add_subdirectory( wpg )
endif()

add_subdirectory( wmf )

if(SHOULD_BUILD_FILTER_KARBON1X_TO_KARBON)
    add_subdirectory( karbon1.x )
endif()

if(SHOULD_BUILD_FILTER_EPS_TO_SVG_AI)
    add_subdirectory( eps )
endif()

if (SHOULD_BUILD_FILTER_PDF_TO_SVG)
    add_subdirectory( pdf )
endif ()

if(SHOULD_BUILD_FILTER_XFIG_TO_ODG)
    add_subdirectory( xfig )
endif()
