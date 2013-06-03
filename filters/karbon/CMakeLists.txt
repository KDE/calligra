
include_directories(
        ${KOTEXT_INCLUDES}
        ${KOMAIN_INCLUDES}
        ${FLAKE_INCLUDES}
        ${CMAKE_SOURCE_DIR}/karbon
        ${CMAKE_SOURCE_DIR}/karbon/common
        ${CMAKE_SOURCE_DIR}/karbon/ui
        ${CMAKE_BINARY_DIR}/karbon
        ${KDE4_INCLUDES}
        ${CMAKE_SOURCE_DIR}/plugins/ )

if(SHOULD_BUILD_FILTER_KARBON_TO_IMAGE)
    add_subdirectory( image )
endif(SHOULD_BUILD_FILTER_KARBON_TO_IMAGE)

add_subdirectory( svg )

if(SHOULD_BUILD_FILTER_WPG_TO_SVG)
  add_subdirectory( wpg )
endif(SHOULD_BUILD_FILTER_WPG_TO_SVG)

add_subdirectory( wmf )

if(SHOULD_BUILD_FILTER_KARBON1X_TO_KARBON)
    add_subdirectory( karbon1.x )
endif(SHOULD_BUILD_FILTER_KARBON1X_TO_KARBON)

if(SHOULD_BUILD_FILTER_EPS_TO_SVG_AI)
    add_subdirectory( eps )
endif(SHOULD_BUILD_FILTER_EPS_TO_SVG_AI)

if (SHOULD_BUILD_FILTER_PDF_TO_SVG)
    add_subdirectory( pdf )
endif (SHOULD_BUILD_FILTER_PDF_TO_SVG)

if(SHOULD_BUILD_FILTER_XFIG_TO_ODG)
    add_subdirectory( xfig )
endif(SHOULD_BUILD_FILTER_XFIG_TO_ODG)
