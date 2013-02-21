add_subdirectory( koplugin )
add_subdirectory( widgetutils )
add_subdirectory( widgets )
add_subdirectory( odf )
add_subdirectory( textlayout )
add_subdirectory( kotext )
add_subdirectory( flake )
add_subdirectory( basicflakes )
add_subdirectory( pigment )
add_subdirectory( main )
add_subdirectory( kundo2 )
add_subdirectory( vectorimage )

if (SHOULD_BUILD_SCRIPTING)
    add_subdirectory( kokross )
endif (SHOULD_BUILD_SCRIPTING)

if (SHOULD_BUILD_FLOW OR SHOULD_BUILD_STAGE)
    add_subdirectory( kopageapp )
endif (SHOULD_BUILD_FLOW OR SHOULD_BUILD_STAGE)

if(SHOULD_BUILD_KOREPORT)
    add_subdirectory( koproperty ) # TODO move to independent place like kdesupport
endif(SHOULD_BUILD_KOREPORT)

if (HAVE_ICU AND HAVE_SQLITE AND SHOULD_BUILD_CALLIGRADB)
    add_subdirectory( db ) # used by Kexi and Word's Biblio; TODO: remove when Predicate lib arrives
endif ()

# only build koreport if possible
if (SHOULD_BUILD_KOREPORT)
    add_subdirectory( koreport )
endif (SHOULD_BUILD_KOREPORT)

if (Soprano_FOUND)
    add_subdirectory( rdf )
endif (Soprano_FOUND)
