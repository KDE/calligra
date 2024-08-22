add_definitions(
  -DTRANSLATION_DOMAIN=\"calligra\"
)

if (SHOULD_BUILD_LIB_CALLIGRA)
    add_subdirectory( plugin )
    add_subdirectory( widgetutils )
    add_subdirectory( widgets )
    add_subdirectory( store )
    add_subdirectory( odf )
    add_subdirectory( textlayout )
    add_subdirectory( text )
    add_subdirectory( flake )
    add_subdirectory( basicflakes )
    add_subdirectory( pigment )
    add_subdirectory( kundo2 )
endif ()

if (SHOULD_BUILD_LIB_KOMAIN)
    add_subdirectory( main )
endif ()

if (SHOULD_BUILD_LIB_KOVECTORIMAGE)
    add_subdirectory( vectorimage )
endif ()

if (SHOULD_BUILD_LIB_KOPAGEAPP)
    add_subdirectory( pageapp )
endif ()

if (SHOULD_BUILD_FEATURE_RDF)
    add_subdirectory( rdf )
endif ()
