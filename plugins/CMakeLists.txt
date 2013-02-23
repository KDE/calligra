if(MSVC)
  # avoid "cannot open file 'LIBC.lib'" error
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBC.LIB")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBC.LIB")
endif(MSVC)


include_directories(
    ${KOMAIN_INCLUDES}
)

if (SHOULD_BUILD_PLUGIN_COLORENGINES)
    macro_optional_add_subdirectory( colorengines )
endif (SHOULD_BUILD_PLUGIN_COLORENGINES)

if (SHOULD_BUILD_PLUGIN_MUSICSHAPE)
    macro_optional_add_subdirectory( musicshape )
endif (SHOULD_BUILD_PLUGIN_MUSICSHAPE)

if (SHOULD_BUILD_PLUGIN_SPACENAVIGATOR)
    macro_optional_find_package(Spnav)
    macro_log_feature(SPNAV_FOUND "Spnav" "3Dconnexion device driver and SDK" "http://spacenav.sourceforge.net/" FALSE "" "Required by SpaceNavigator 3D mouse plugin") 
    IF (SPNAV_FOUND)
        macro_optional_add_subdirectory( spacenavigator )
    ENDIF (SPNAV_FOUND)
endif (SHOULD_BUILD_PLUGIN_SPACENAVIGATOR)

if (SHOULD_BUILD_PLUGIN_ARTISTICTEXTSHAPE)
    macro_optional_add_subdirectory( artistictextshape )
endif (SHOULD_BUILD_PLUGIN_ARTISTICTEXTSHAPE)

if (SHOULD_BUILD_PLUGIN_DOCKERS)
    macro_optional_add_subdirectory( dockers )
endif (SHOULD_BUILD_PLUGIN_DOCKERS)

if (SHOULD_BUILD_PLUGIN_TEXTEDITING AND SHOULD_BUILD_TEXTSHAPE)
    macro_optional_add_subdirectory( textediting )
endif(SHOULD_BUILD_PLUGIN_TEXTEDITING AND SHOULD_BUILD_TEXTSHAPE)

if (SHOULD_BUILD_PLUGIN_STAGING)
        macro_optional_add_subdirectory( staging )
endif (SHOULD_BUILD_PLUGIN_STAGING)

if(SHOULD_BUILD_PLUGIN_KEXI AND SHOULD_BUILD_KEXI)
        macro_optional_add_subdirectory( kexi )
endif(SHOULD_BUILD_PLUGIN_KEXI AND SHOULD_BUILD_KEXI)

if (SHOULD_BUILD_PLUGIN_DEFAULTTOOLS)
    macro_optional_add_subdirectory( defaultTools )
endif (SHOULD_BUILD_PLUGIN_DEFAULTTOOLS)

if (SHOULD_BUILD_PLUGIN_PATHSHAPES)
    macro_optional_add_subdirectory( pathshapes )
endif (SHOULD_BUILD_PLUGIN_PATHSHAPES)

if (SHOULD_BUILD_PLUGIN_VARIABLES)
    macro_optional_add_subdirectory( variables )
endif (SHOULD_BUILD_PLUGIN_VARIABLES)

if (SHOULD_BUILD_PLUGIN_CHARTSHAPE)
    macro_optional_add_subdirectory( chartshape )
endif (SHOULD_BUILD_PLUGIN_CHARTSHAPE)

if (SHOULD_BUILD_PLUGIN_PICTURESHAPE)
    macro_optional_add_subdirectory( pictureshape )
endif (SHOULD_BUILD_PLUGIN_PICTURESHAPE)

if (SHOULD_BUILD_PLUGIN_TEXTSHAPE AND SHOULD_BUILD_TEXTSHAPE)
    macro_optional_add_subdirectory( textshape )
endif(SHOULD_BUILD_PLUGIN_TEXTSHAPE AND SHOULD_BUILD_TEXTSHAPE)

if (SHOULD_BUILD_PLUGIN_PLUGINSHAPE)
    macro_optional_add_subdirectory( pluginshape )
endif (SHOULD_BUILD_PLUGIN_PLUGINSHAPE)

if (SHOULD_BUILD_PLUGIN_FORMULASHAPE)
    macro_optional_add_subdirectory( formulashape )
endif (SHOULD_BUILD_PLUGIN_FORMULASHAPE)

if (SHOULD_BUILD_PLUGIN_VECTORSHAPE)
    # ??? old comment: as the vector shape depends on filters subdir (not really intuitive) lets allow disabling
    macro_optional_add_subdirectory( vectorshape )
endif (SHOULD_BUILD_PLUGIN_VECTORSHAPE)

# only build the reporting plugins if building koreport
if(SHOULD_BUILD_PLUGIN_REPORTING AND SHOULD_BUILD_KOREPORT)
    macro_optional_add_subdirectory( reporting )
endif(SHOULD_BUILD_PLUGIN_REPORTING AND SHOULD_BUILD_KOREPORT)

#macro_optional_add_subdirectory( videoshape )
#macro_optional_add_subdirectory( commentshape )
